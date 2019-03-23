require 'test/unit'
require 'net/http'
require '../spider.rb'



class TestLjApi < Test::Unit::TestCase
  
  def setup
    @lj_spider = LjSpider.new
  end

  def test_random_journal
    journal = @lj_spider.random_journal
    
    p journal
    assert_not_nil(journal)
  end
  
  def test_friends
    u = "blueella"
    f = @lj_spider.friends(u)
    
    p f
    assert_not_nil(f)
    assert_not_true(f.empty?)
  end

  
  def test_mysql
    Mysql.open('jkan', 'blah', 'lj') do |mysql|
      collection = []
      mysql.query("select b from friend_of where a = 'deese'") do |result|
        result.each do |array|
          collection.concat(array)
        end
      end
      p collection
    end
  end

  def test_mysql_result
    Mysql.open('jkan', 'blah', 'lj') do |mysql|
      mysql.query("select b from friend_of where a = 'deese'") do |result|
        collection = result.fetch_all
        p collection
        p collection.flatten
      end
    end
  end

  def test_mysql_insert
    Mysql.open('jkan', 'blah', 'lj') do |mysql|
      mysql.query("select b from friend_of where a = 'deese'") do |result|
        collection = result.fetch_all
        p collection
        p collection.flatten
      end
    end
  end

end