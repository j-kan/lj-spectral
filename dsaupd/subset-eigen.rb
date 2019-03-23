filename   = ARGV[0]
num_values = ARGV[1].to_i

File.open(filename) do |f|
    f.each_line do |l|
        eigs = l.split(/ /, num_values+1)
        eigs.pop
        puts eigs.join(' ')
    end
end
