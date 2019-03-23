require 'test/unit'
require 'mysql-helper.rb'
require 'lj_db.rb'


TABLE_DDL = <<-END_DDL

CREATE TABLE tmp (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name varchar(16) NOT NULL,
  visited bit default 0,
  CONSTRAINT UNIQUE KEY i_tmp_name (name),
  KEY i_tmp_visited (visited)
) ENGINE=MyISAM;

END_DDL



class TestLjDb < Test::Unit::TestCase
  
  def setup
#    Lj::Db.open { |db| db.exec(TABLE_DDL) }
  end
  
  def teardown
#    Lj::Db.open { |db| db.exec("DROP TABLE IF EXISTS tmp") }
  end

  
  def test_max_id
    Lj::Db.open do |db|
      db.exec("DROP TABLE IF EXISTS tmp")
      db.exec(TABLE_DDL)
      db.exec("INSERT INTO tmp(name, visited) VALUES ('blargh',1),('urgh',0),('eeow',0),('ur',0),('erm',1),('gurf',0)")
      db.exec("DELETE FROM tmp WHERE name LIKE 'ur%'")

      assert_equal(6, db.max_id("tmp"))
    end
  end

  def test_next_id
    Lj::Db.open do |db|
      assert_equal(3, db.next_id(1, "tmp"))
    end
  end

end