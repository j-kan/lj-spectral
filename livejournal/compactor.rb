require 'lj_db.rb'

module Lj
  
  class Compactor

    def compact
      Lj::Db.open do |db|
        max_id = db.max_id
        puts "max: #{max_id}"
        #max_id = 1000
        id = 0
        new_id = 0
        while id < max_id
          id = db.next_id(new_id)
          new_id += 1
          if (id > new_id)
            puts "#{id} => #{new_id}"
            db.change_id(new_id, id)
            #break
          end
        end
      end
    end

  end
end



if __FILE__ == $0
  compactor = Lj::Compactor.new
  compactor.compact
end
