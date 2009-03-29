# Copyright (c) 2009 Johan Sørensen
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# 'Software'), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

module SimpleMmap
  # Represents a window into a mmap()'ed file, using a familiar index syntax
  # for getting single bytes or Ranges of bytes within that FileWindow
  class FileWindow
    # Create a mmap'ed window for file at +path+
    # You are responsible for closing it when you're done using #close
    def initialize(path)
      @path = path
      @offset = 0
      @mmap = SimpleMmap::MappedFile.new(@path)
    end
    attr_reader :path, :offset
    
    # unmaps the mmap'ed file
    def close
      @mmap.close
      @mmap = nil
    end
  
    # The current offset
    def offset
      @offset
    end
    alias_method :pos, :offset
  
    # Set the current offset to +new_offset+
    def offset=(new_offset)
      @offset = new_offset
    end
    alias_method :pos=, :offset=
    
    # Move the current offset to +to_pos+
    def seek(to_pos)
      self.pos = to_pos
    end
  
    # Read data from the mmap'ed file. Takes the same arguments as 
    # Array/String#[] (eg [2], [2,5], [2..5] or [2...5])
    # The current offset will be tracked accordingly
    def [](*index)
      if index.length == 1
        index = index.first
      end
      case index
      when Array
        offset, length = index
      when Fixnum
        offset = index
        length = 0
      when Range
        offset = index.begin
        length = index.end - index.begin
        unless index.exclude_end?
          length += 1
        end
      end
      
      @offset = offset + length
    
      if length.zero?
        @mmap.read_window_data(offset, 1)
      else
        @mmap.read_window_data(offset, length)
      end
    end
    
    # Read +length+ bytes starting from the current offset
    def read(length)
      data = @mmap.read_window_data(@offset, length)
      @offset += length
      data
    end
  end
end
