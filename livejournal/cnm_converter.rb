require 'lj_db.rb'

module Lj
  
  class CnmConverter
    
    BATCH_SIZE = 1000
    
    def convert
      File.open("is_friend_of.pairs", "w+") do |outf|
        Lj::Db.open do |db|
          num_written = 11694830 # 1254830
          loop do
            batch = db.get_batch("is_friend_of", num_written, BATCH_SIZE)
            break if batch.empty?
            batch.each {|a,b| outf.puts "#{a}\t#{b}" }
            num_written += batch.size
          end
        end
      end
    end
    
  end
end



if __FILE__ == $0
  converter = Lj::CnmConverter.new
  converter.convert
end
