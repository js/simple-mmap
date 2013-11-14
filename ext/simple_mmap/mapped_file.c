/* Copyright (c) 2009 Johan Sørensen
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include "ruby.h"

#ifndef NUM2SIZET
#  include <limits.h>
#  include <stdint.h>
#  if SIZE_MAX > ULONG_MAX
#    define NUM2SIZET NUM2ULL
#    define SIZET2NUM ULONG2NUM
#  elif SIZE_MAX > UINT_MAX
#    define NUM2SIZET NUM2ULONG
#    define SIZET2NUM ULONG2NUM
#  elif SIZE_MAX > USHRT_MAX
#    define NUM2SIZET NUM2UINT
#    define SIZET2NUM UINT2NUM
#  else
#    define NUM2SIZET NUM2USHORT
#    define SIZET2NUM USHORT2NUM
#  endif
#endif

// a memory mapped file
typedef struct {  
	int fd;
  caddr_t map;
	size_t len;
  caddr_t read_base;
  size_t read_len;
} simple_mmap_map;

static VALUE mod_simple_mmap;
static VALUE sm_mapped_file;
static VALUE sm_map_data;

/*
 * Document-method: new
 * call-seq: SimpleMMap::MappedFile.new(path)
 *
 * mmap() the file at +path+
 */
static VALUE sm_mapped_file_initialize(int argc, VALUE *argv, VALUE vself)
{
  int fd = -1;
  size_t length;
  struct stat st;
  caddr_t base = (caddr_t) - 1;
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
  VALUE filename, voffset, vlength;
  off_t offset, offset_mod;

  rb_scan_args(argc, argv, "12", &filename, &voffset, &vlength);
  if (!NIL_P(voffset) && NUM2LL(voffset) < 0)
    rb_raise(rb_eRangeError, "offset out of range: %lld", NUM2LL(voffset));
  if (!NIL_P(vlength) && NUM2LL(vlength) < 0)
    rb_raise(rb_eRangeError, "length out of range: %lld", NUM2LL(vlength));

  fd = open(RSTRING_PTR(filename), O_RDONLY);
  if (fd == -1) {
    rb_raise(rb_eArgError, "Failed to open file %s", RSTRING_PTR(filename));
    close(fd);
  }
  
  // Get the size of the file
  if (fstat(fd, &st) == -1) {
    rb_raise(rb_eArgError, "Failed to stat file %s", RSTRING_PTR(filename));
    close(fd);
  }
  offset = NIL_P(voffset) ? 0 : NUM2SIZET(voffset);
  length = NIL_P(vlength) ? st.st_size : NUM2SIZET(vlength);
  if (offset + length > st.st_size) length = st.st_size - offset;
  offset_mod = offset % sysconf(_SC_PAGESIZE);
  offset = offset - offset_mod;
  length = length + offset_mod;
  
  // do the mmap
  base = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, offset);
  if (base == (caddr_t) -1) {
    rb_raise(rb_eArgError, "Failed to mmap file %s", RSTRING_PTR(filename));
    close(fd);
  }
  
  // set the mmap structure as an ivar
  sm_map = ALLOC(simple_mmap_map);
  vsm_map = Data_Wrap_Struct(sm_map_data, NULL, free, sm_map);
  sm_map->fd = fd;
  sm_map->map = base;
  sm_map->len = length;
  sm_map->read_base = base + offset_mod;
  sm_map->read_len = length - offset_mod;
  rb_ivar_set(vself, rb_intern("@mmap_data"), vsm_map);
  
  return Qnil;
}

/*
 * Document-method: close
 * call-seq: obj.close
 *
 * munmap() the current mmapped file
 */
static VALUE sm_mapped_file_close(VALUE vself)
{
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
  sm_map = ALLOC(simple_mmap_map);
  vsm_map = rb_ivar_get(vself, rb_intern("@mmap_data"));
  Data_Get_Struct(vsm_map, simple_mmap_map, sm_map);
  
  if (sm_map->map != (caddr_t) -1)
    munmap(sm_map->map, sm_map->len);
  if (sm_map->fd != -1)
    close(sm_map->fd);
  
  return Qtrue;
}

/*
 * Document-method: read_window_data
 * call-seq: obj.read_window_data(offset, length)
 *
 * Read +length+ bytes starting at +offset+
 */
static VALUE sm_mapped_file_read_window_data(VALUE vself, VALUE voffset, VALUE vlength) 
{
  size_t offset, length;
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
  if (NUM2LL(voffset) < 0) return Qnil;
  if (NUM2LL(vlength) < 0) rb_raise(rb_eRangeError, "length out of range: %lld", NUM2LL(vlength));
  offset = NUM2SIZET(voffset);
  length = NUM2SIZET(vlength);

  vsm_map = rb_ivar_get(vself, rb_intern("@mmap_data"));
  Data_Get_Struct(vsm_map, simple_mmap_map, sm_map);
  
  if (offset > sm_map->read_len) return Qnil;

  // If the range overflows, return part that overlaps
  if ((offset + length) > sm_map->read_len) {
    length = sm_map->read_len - offset;
  }

  return rb_str_new(sm_map->read_base + offset, length);
}

/*
 * Document-method: size
 * call-seq: obj.size
 *
 * Return size of mapped file
 */

static VALUE sm_mapped_file_size(VALUE vself)
{
  VALUE vsm_map;
  simple_mmap_map *sm_map;

  vsm_map = rb_ivar_get(vself, rb_intern("@mmap_data"));
  Data_Get_Struct(vsm_map, simple_mmap_map, sm_map);
  return SIZET2NUM(sm_map->read_len);
}

void Init_mapped_file()
{
  mod_simple_mmap = rb_define_module("SimpleMmap");
  
  sm_mapped_file = rb_define_class_under(mod_simple_mmap, "MappedFile", rb_cObject);
  rb_define_private_method(sm_mapped_file, "initialize", sm_mapped_file_initialize, -1);
  rb_define_method(sm_mapped_file, "close", sm_mapped_file_close, 0);
  rb_define_method(sm_mapped_file, "read_window_data", sm_mapped_file_read_window_data, 2);
  rb_define_method(sm_mapped_file, "size", sm_mapped_file_size, 0);
  
  sm_map_data = rb_define_class_under(sm_mapped_file, "MmapData", rb_cObject);
}
