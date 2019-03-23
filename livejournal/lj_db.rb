require 'mysql-helper.rb'
require 'lj_network.rb'

=begin
CREATE DATABASE lj;

DROP TABLE IF EXISTS user;
CREATE TABLE user (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name varchar(16) NOT NULL,
  visited bit default 0,
  CONSTRAINT UNIQUE KEY i_user_name (name),
  KEY i_user_visited (visited)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS interest;
CREATE TABLE interest (
  id INT NOT NULL PRIMARY KEY,
  name varchar(80) NOT NULL,
  count INT default 0,
  CONSTRAINT UNIQUE KEY i_interest_name (name)
) ENGINE=MyISAM;




DROP TABLE IF EXISTS community;
CREATE TABLE community (
  id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name varchar(16) NOT NULL,
  CONSTRAINT UNIQUE KEY i_community_name (name)
) ENGINE=MyISAM;


DROP TABLE IF EXISTS is_friend_of;
CREATE TABLE is_friend_of (
  a int NOT NULL,
  b int NOT NULL,
  KEY i_is_friend_of_a (a), 
  KEY i_is_friend_of_b (b)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS is_interested_in;
CREATE TABLE is_interested_in (
  user_id     int NOT NULL,
  interest_id int NOT NULL,
  KEY i_is_interested_in_u (user_id), 
  KEY i_is_interested_in_i (interest_id)
) ENGINE=MyISAM;

DROP TABLE IF EXISTS is_member_of;
CREATE TABLE is_member_of (
  user_id      int NOT NULL,
  community_id int NOT NULL,
  KEY i_is_member_of_u (user_id), 
  KEY i_is_member_of_c (community_id)
) ENGINE=MyISAM;





DROP TABLE IF EXISTS friend_of;
CREATE TABLE friend_of (
  a varchar(16) NOT NULL,
  b varchar(16) NOT NULL,
  KEY index_friend_of_a (a), 
  KEY index_friend_of_b (b)
) ENGINE=MyISAM;


=end

class Range
  alias :min :begin
  
  def max
    self.exclude_end? ? self.end-1 : self.end
  end
end

module Lj

  class Db

    def self.open
      Mysql.open('jkan', 'blah', 'lj') do |mysql|
        db = self.new(mysql)
        yield db
      end
    end
    
    def initialize(mysql)
      @mysql = mysql
    end
    
    def exec(sql)
      @mysql.query(sql)
#      @mysql.affected_rows
    end
    
    def query(sql, &block) 
      @mysql.query(sql) { |result| block.call(result) }
    end
    
    def add_users(names)
      values = names.map {|name| "('#{name}')"}
      exec("INSERT INTO user(name) VALUES #{values.join(',')} ON DUPLICATE KEY UPDATE name = name")
    end 
    
    def quote_str(str)
      str.gsub(/'/, "''").gsub(/\\/,'\\\\\\\\')
    end
    
    def add_interests(interests)
      values = interests.map {|interest| "(#{interest.id},'#{quote_str(interest.name[0...80])}',#{interest.count})"}
      exec("INSERT INTO interest(id, name, count) VALUES #{values.join(',')} ON DUPLICATE KEY UPDATE name = name")
    end
    
    def name_from_id (id, table="user")
      query("SELECT name FROM #{table} WHERE id = '#{id}'") do |result|
        return result.fetch_all[0]
      end
    end
    
    def id_from_name (name, table="user")
      query("SELECT id FROM #{table} WHERE name = '#{name}'") do |result|
        return result.fetch_all[0]
      end
    end

    # expects a numeric ID
    def old_friend_edge_list(u)
      edge_list       = FriendEdgeList.new
      
      query("SELECT name FROM is_friend_of LEFT JOIN user ON b = user.id WHERE a = '#{u}'") do |result|
        edge_list.outbound = result.fetch_all
      end
      query("SELECT name FROM is_friend_of LEFT JOIN user ON a = user.id WHERE b = '#{u}'") do |result|
        edge_list.inbound  = result.fetch_all
      end
      
      edge_list
    end
    
    # u is expected to be a database ID, while edge_list contains names (strings)
    def add_friend_edge_list(u, edge_list)

      add_users(edge_list.all_edges)

      unless edge_list.inbound.empty?
        inbound_sql  = edge_list.inbound.map {|f| "'#{f}'"}
        exec("INSERT INTO is_friend_of(a,b) SELECT id, #{u} FROM user WHERE name IN (#{inbound_sql.join(',')})")
      end

      unless edge_list.outbound.empty?
        outbound_sql = edge_list.outbound.map {|f| "'#{f}'"}
        exec("INSERT INTO is_friend_of(a,b) SELECT #{u}, id FROM user WHERE name IN (#{outbound_sql.join(',')})")
      end
      
      # @mysql.affected_rows
    end

    def add_user_interests(u, interests)
      unless interests.empty?
        add_interests(interests) 

        interests_sql  = interests.map {|i| "'#{quote_str(i.name)}'"}
        exec("INSERT INTO is_interested_in(user_id,interest_id) SELECT #{u},id FROM interest WHERE name IN (#{interests_sql.join(',')})")
      end
      # @mysql.affected_rows
    end

    def unexplored
      query('SELECT id FROM user WHERE NOT visited') do |result|
        return result.fetch_all
      end
    end
  
    def next_unexplored(batch_size)
      puts "get next batch of #{batch_size} users"
      query("SELECT id FROM user WHERE NOT visited LIMIT #{batch_size}") do |result|
        return result.fetch_all
      end
    end

    def mark_visited(u)
      exec("UPDATE user SET visited = true WHERE id = #{u}")
    end 

    def max_id(table="user")
      query("SELECT MAX(id) FROM #{table}") do |result|
        return result.fetch_all[0].to_i
      end
    end
    
    def next_id(cur_id, table="user")
      query("SELECT MIN(id) FROM #{table} WHERE id > #{cur_id}") do |result|
        return result.fetch_all[0].to_i
      end
    end
    
    def change_id(new_id, old_id)
      exec("UPDATE user SET id = #{new_id} WHERE id = #{old_id}")
      exec("UPDATE is_friend_of SET a = #{new_id} WHERE a = #{old_id}")
      exec("UPDATE is_friend_of SET b = #{new_id} WHERE b = #{old_id}")
      exec("UPDATE is_interested_in SET user_id = #{new_id} WHERE user_id = #{old_id}")
    end
    
    def get_batch(table, limit_begin, limit_count)
      query("SELECT * FROM #{table} LIMIT #{limit_begin}, #{limit_count}") do |result|
        return result.fetch_all(false)
      end
    end
    
    def get_net_for_range(r)
      query("SELECT * FROM is_friend_of WHERE a >= #{r.min} AND a <= #{r.max} AND b >= #{r.min} AND b <= #{r.max}") do |result|
        return result.fetch_all(false)
      end
    end
    
    def get_user_interests(u)
      query("SELECT interest_id FROM is_interested_in_sample WHERE user_id = #{u}") do |result|
        return result.fetch_all
      end
    end
  end

end