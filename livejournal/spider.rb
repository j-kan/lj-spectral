require 'net/http'
require 'stringio'
require 'lj_network.rb'
require 'lj_db.rb'



module Lj

  class Spider

    LJ_BASE_URL  = "www.livejournal.com"
    RANDOM_URL   = "/random.bml"

    UA_HASH      = {'User-Agent' => 'xxx@gmail.com'}

    def lj_get(url)
      tries = 0
      begin
        http = Net::HTTP.new(LJ_BASE_URL, 80)
        http.get(url, UA_HASH) do |body|
          yield body
        end
      rescue => err
        puts "Error: #{err}"
        if (tries < 10)
          tries += 1
          Kernel.sleep(0.5 + tries/2.0)
          retry
        else
          raise
        end
      end
    end

    def random_journal
      lj_get(RANDOM_URL) do |str|
        if str =~ /http:\/\/([a-z0-9\-_]{1,16})\.livejournal.com/
          return $1
        end
      end
      nil
    end

    # expects u to be a string
    def friend_edge_list(u)
      friends_url = "/misc/fdata.bml?user=#{u}"
      edge_list   = FriendEdgeList.new

      lj_get(friends_url) do |body|
        StringIO.new(body).each_line do |line|
          case line
            when /> ([a-z0-9_]{1,16})/    # -- i.e. u is-friend-of $1
              edge_list.outbound << $1
            when /< ([a-z0-9_]{1,16})/    # -- i.e. $1 is-friend-of u
              edge_list.inbound << $1
          end
        end
      end

      edge_list
    end

    # expects u to be a string
    def interest_list(u)
      interest_url = "/misc/interestdata.bml?user=#{u}"
      interests   = []

      lj_get(interest_url) do |body|
        StringIO.new(body).each_line do |line|
          if line =~ /^(\d+)\s(\d+)\s(.+)\n$/
            interests << Interest.new($1, $3, $2)
          end
        end
      end

      interests
    end
  end



  class Walker

    BATCH_SIZE = 100

    def initialize
      @spider = Spider.new
    end

    def next_batch(db, size = BATCH_SIZE)
      frontier = db.next_unexplored(size)
      if frontier.empty?
        frontier << @spider.random_journal
        db.add_users(frontier)
        frontier = db.next_unexplored(1)
      end
      frontier
    end

    def walk
      Db.open do |db|
        frontier = next_batch(db)
        while !frontier.empty?
          visit_one(db, frontier.pop)
          frontier = next_batch(db) if frontier.empty?
        end
      end
    end


    def visit_one(db, u)
      u_name = db.name_from_id(u)

      puts "\n==========> #{u}: #{u_name}"

      edge_list     = @spider.friend_edge_list(u_name)
      interests     = @spider.interest_list(u_name)
      old_edge_list = db.old_friend_edge_list(u)

      puts "\nWWW",     edge_list
      puts "\nDB",  old_edge_list
      puts "\nINT", (interests.map {|i| i.name}).join(' | ')

      edge_list -= old_edge_list
      edge_list.remove(u_name)   # remove narcissistic edges

      #puts "new:",   edge_list

      db.add_friend_edge_list(u, edge_list) unless edge_list.empty?
      db.add_user_interests(u, interests)   unless interests.empty?
      db.mark_visited(u)

      edge_list
    end
  end

end


if __FILE__ == $0
  walker = Lj::Walker.new
  walker.walk
end
