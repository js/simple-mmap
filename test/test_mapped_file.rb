require "test/unit"
require "tempfile"

require File.dirname(__FILE__) + "/../lib/simple_mmap"

class TestMappedFile < Test::Unit::TestCase
  def setup
    @file = Tempfile.new("TestMappedFile.data")
    File.open(@file.path, "w"){|f| f.puts(('a'..'z').to_a.join) }
    @map = SimpleMmap::MappedFile.new(@file.path)
  end
  
  def teardown
    @map.close
  end
  
  def test_read_window_data
    assert_equal "abc", @map.read_window_data(0, 3)
    assert_equal "bc", @map.read_window_data(1, 2)
    assert_equal "klmno", @map.read_window_data(10, 5)
  end
  
  def test_read_window_beyond_buffer_size
    assert_equal "z\n", @map.read_window_data(25, 5)
    assert_nil @map.read_window_data(28, 5)
  end
end
