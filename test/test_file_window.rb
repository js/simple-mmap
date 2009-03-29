require "test/unit"

require File.dirname(__FILE__) + "/../lib/simple_mmap"

class TestFileWindow < Test::Unit::TestCase
  def setup
    @file = Tempfile.new("TestFileWindow.data")
    File.open(@file.path, "w"){|f| f.puts(('a'..'z').to_a.join) }
    @fw = SimpleMmap::FileWindow.new(@file.path)
  end
  
  def teardown
    @fw.close
  end
  
  def test_should_start_at_offset_0
    assert_equal 0, @fw.offset
    assert_equal 0, @fw.pos
  end
  
  def test_should_be_able_to_set_the_offset
    @fw.offset = 2
    assert_equal 2, @fw.offset
    assert_equal 2, @fw.pos
    
    @fw.pos = 4
    assert_equal 4, @fw.offset
    assert_equal 4, @fw.pos
    
    @fw.seek(6)
    assert_equal 6, @fw.offset
    assert_equal 6, @fw.pos
  end
  
  def test_get_single_byte_at_current_offset_with_index
    assert_equal "c", @fw[2]
    assert_equal 2, @fw.offset
    assert_equal "b", @fw[1]
    assert_equal 1, @fw.offset
  end
  
  def test_get_from_x_to_y_with_index_comma_notation
    assert_equal "cde", @fw[2, 3]
    assert_equal 5, @fw.pos
    assert_nil @fw[28, 2]    
  end
  
  def test_get_with_range_including
    assert_equal "cde", @fw[2..4]
    assert_equal 5, @fw.offset    
  end
  
  def test_get_with_range_excluding
    assert_equal "cd", @fw[2...4]
    assert_equal 4, @fw.offset    
  end
  
  def test_read
    assert_equal "abc", @fw.read(3)
    assert_equal 3, @fw.offset
  end
end
