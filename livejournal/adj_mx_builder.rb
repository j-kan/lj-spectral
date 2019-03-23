require 'lj_db.rb'
require 'matrix'
require 'mathn'

module Lj
  
  class AdjacencyMatrixBuilder
    
    ID_RANGE = 1..50
    
    def digraph(range = ID_RANGE)
      adj = Hash.new(0)
      Lj::Db.open do |db|
        db.get_net_for_range(range).each do |a,b|
          puts "#{a} => #{b}"
          adj[[a.to_i, b.to_i]]=1
        end
      end
      p adj
      @adj = adj
      @range = range
      adj
    end
    
    def graph(range = ID_RANGE)
      adj = Hash.new(0)
      Lj::Db.open do |db|
        db.get_net_for_range(range).each do |a,b|
          a,b = a.to_i, b.to_i
          a,b = b,a if b < a
          puts "#{a} <=> #{b}"
          adj[[a, b]]=1
        end
      end
      p adj
      @adj = adj
      @range = range
      adj
    end
    
    def write(filename)
      File.open(filename, "w") do |outf|
        @range.each do |i|
          @range.each do |j|
            outf.print(@adj[[i,j]], "\t")
          end
          outf.puts
        end
      end
    end
  end
  
end



if __FILE__ == $0
  builder = Lj::AdjacencyMatrixBuilder.new
  builder.graph(1..50)
  builder.write("undirected-adjacency-matrix.tab")
end
