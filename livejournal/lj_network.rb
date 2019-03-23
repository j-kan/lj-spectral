
module Lj

  # Inbound:  edges from others linked to this user as a friend
  # Outbound: edges from this user to other users
  # edges are expressed as strings, not database IDs
  class FriendEdgeList
    attr_accessor :inbound, :outbound
    
    def initialize
      @inbound = []
      @outbound = []
    end
    
    def initialize_copy(original)
      @inbound  = original.inbound.dup
      @outbound = original.outbound.dup
    end
    
    # Deletes every element that appears in the given enumerable object
    # and returns self.
    def subtract(edge_list)
      @inbound  -= edge_list.inbound
      @outbound -= edge_list.outbound
      self
    end
    
    def -(edge_list)
      dup.subtract(edge_list)
    end
    
    def remove(u)
      @inbound.delete u
      @outbound.delete u
    end
    
    def empty?
      @inbound.empty? and @outbound.empty?
    end
    
    def all_edges
      @inbound | @outbound
    end
    
    def to_s
      "  <<IN<<<  #{@inbound.join(' | ')}\n" +
      "  >>OUT>>  #{@outbound.join(' | ')}"
    end
  end
  
  
  class Interest
    attr_accessor :id, :name, :count
    
    def initialize(id, name, count)
      @id = id.to_i
      @name = name.chomp
      @count = count.to_i
    end
  end
  
end