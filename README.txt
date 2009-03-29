= simple_mmap

http://simple-mmap.rubyforge.org/

== DESCRIPTION:

  A simplistic interface for reading memory mapped files

== FEATURES/PROBLEMS:

* mmap a file. Read data from it. Close it when you're done. More fun than the "ball in a cup" game.
* it ONLY supports reading mmap'ed files, not writing to the mmap'ed region

== SYNOPSIS:
  
>> fw = SimpleMmap::FileWindow.open("path/to/large/file")
>> fw[23467, 200] #=> data between byte 23467 and the next 200 bytes
# works with the same semantics as Array#[] (eg. fw[10..14] and fw[10...14])
>> fw.close # you MUST close the FileWindow after you're done (HAHA no blocks for you yet!)


== REQUIREMENTS:

* A mmap() capable platform.

== INSTALL:

$ gem install simple_mmap
# If you have ruby 1.9+ built with executables having a program-suffix:
$ env rake=rake1.9 gem install simple_mmap

== LICENSE:

(The MIT License)

Copyright (c) 2009 Johan Sørensen

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
