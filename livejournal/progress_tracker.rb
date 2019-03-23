require 'mysql-helper.rb'

SQL = "SELECT COUNT(*),visited FROM user GROUP BY visited=true"

class ProgressChecker

  def check
    progress = {}
    Mysql.open('jkan', 'blah', 'lj') do |db|
      db.query(SQL) do |result| 
        counts = result.fetch_all(false)
        counts.each do |count, visited| 
          progress[visited == "\001" ? "visited" : "frontier"] = count
        end
      end        
    end
    puts "#{progress['visited']},#{progress['frontier']}"
  end
  
  
end


if __FILE__ == $0
  tracker = ProgressChecker.new
  while 1
    tracker.check
    $stdout.flush
    Kernel.sleep(60 * 5)
  end
end
