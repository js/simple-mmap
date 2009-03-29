require 'mkmf'

dir_config('mapped_file')
have_func('mmap')
create_makefile('mapped_file')
