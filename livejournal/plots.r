load.from.db = function(tablename) 
{
	m   = dbDriver("MySQL")
	con = dbConnect(m, user="jkan", password="blah", dbname="lj", host="localhost")
	dbReadTable(con, tablename)
}


makeplot = function(dir, basename, dataframe) 
{
	pngpath  = paste(dir, basename, ".png", sep="")
	
	print(pngpath)

	png(file=pngpath, width=1000, height=1000)
	xyplot(log(degree) ~ log(n), dataframe)
}


dir = "g:/hack/livejournal/img/"

in.degree.histogram = load.from.db("in_degree_histogram")
out.degree.histogram = load.from.db("out_degree_histogram")

makeplot(dir, "in.degree.histogram", in.degree.histogram)
makeplot(dir, "out.degree.histogram", out.degree.histogram)

graphics.off()
