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

// a memory mapped file
typedef struct {  
	int fd;
  caddr_t map;
	size_t len;
} simple_mmap_map;

static VALUE mod_simple_mmap;
static VALUE sm_mapped_file;
static VALUE sm_map_data;

static VALUE sm_mapped_file_initialize(VALUE vself, VALUE filename)
{
  int fd = -1;
  size_t length;
  struct stat st;
  caddr_t base = (caddr_t) - 1;
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
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
  length = st.st_size;
  
  // do the mmap
  base = mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0);
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
  rb_ivar_set(vself, rb_intern("@mmap_data"), vsm_map);
  
  return Qnil;
}

static VALUE sm_mapped_file_close(VALUE vself)
{
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
  sm_map = ALLOC(simple_mmap_map);
  vsm_map = rb_ivar_get(vself, rb_intern("@mmap_data"));
  Data_Get_Struct(vsm_map, simple_mmap_map, sm_map);
  // TODO: check that the mmap+fd is active first
  munmap(sm_map->map, sm_map->len);
  close(sm_map->fd);
}

static VALUE sm_mapped_file_read_window_data(VALUE vself, VALUE voffset, VALUE vlength) 
{
  size_t offset = NUM2INT(voffset);
  size_t length = NUM2INT(vlength);
  char buff[length];
  VALUE vsm_map;
  simple_mmap_map *sm_map;
  
  sm_map = ALLOC(simple_mmap_map);
  vsm_map = rb_ivar_get(vself, rb_intern("@mmap_data"));
  Data_Get_Struct(vsm_map, simple_mmap_map, sm_map);
  
  if (offset > sm_map->len) {
    return Qnil;
  }
  
  size_t curr;
  curr = offset;
  size_t i;
  for(i = 0; i < length; ++i) {
    //printf("i=%i offset=%i length=%i curr=%i map->len=%i\n", i, offset, length, curr, sm_map->len);
    if ((curr + i) > sm_map->len)
      break;
    buff[i] = sm_map->map[curr++];
  }
  
  if ((offset + length) > sm_map->len)
    return rb_str_new(buff, ((offset+length) - sm_map->len)-1);
      
  return rb_str_new(buff, length);
}

void Init_mapped_file()
{
  mod_simple_mmap = rb_define_module("SimpleMmap");
  
  sm_mapped_file = rb_define_class_under(mod_simple_mmap, "MappedFile", rb_cObject);
  rb_define_private_method(sm_mapped_file, "initialize", sm_mapped_file_initialize, 1);
  rb_define_method(sm_mapped_file, "close", sm_mapped_file_close, 0);
  rb_define_method(sm_mapped_file, "read_window_data", sm_mapped_file_read_window_data, 2);
  
  sm_map_data = rb_define_class_under(sm_mapped_file, "MmapData", rb_cObject);
}
