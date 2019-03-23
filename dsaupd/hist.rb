def get_assignments_one_line(filename)

    ass = []

    File.open(filename) do |f|
	f.each_line do |l|
		ass = l.split /\s/
        end
    end

    count = Hash.new(0)

    ass.each do |a|
	count[a.to_i] += 1
    end

    return ass, count
end


def get_assignments(filename)

    ass   = []
    count = Hash.new(0)

    File.open(filename) do |f|
	f.each_line do |l|
		ass << l
		count[l.to_i] += 1
        end
    end

    return ass, count
end

if __FILE__ == $0
  ARGV.each do |filename|
  	ass, counts = get_assignments(filename)
  	puts filename
        p counts
	p counts.values.sort
	p counts.keys.sort
  end
end
