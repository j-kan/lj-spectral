require 'mysql'

# A few bits to make Mysql easier to use
#
class Mysql

  alias :raw_query :query

  def query(sql, &block)
    begin
      if block.nil?
        raw_query(sql)
      else
        raw_query(sql) { |result| block.call(result) }
      end
    rescue => err
      puts err
      puts sql
      raise
    end
  end

  
  def self.open(u, pw, db, host = 'localhost')
    begin
      mysql = self.new(host, u, pw, db)
      yield mysql
    ensure
      mysql.close  if mysql
    end
  end
  
  class Result
    def fetch_all(flat = true)
      collection = []
      self.each do |array|
          collection << array
      end
      flat ? collection.flatten : collection
    end
  end
  
end

