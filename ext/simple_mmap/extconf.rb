require 'mkmf'

have_func('mmap')
create_makefile('simple_mmap/mapped_file')
