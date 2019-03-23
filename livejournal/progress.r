state = read.table("./progress.txt", sep=",", col.names=c("visited","frontier"))

trajectory = data.frame(1:length(state$visited), state$visited, state$frontier)
names(trajectory) = c("n","visited","frontier")


v.diff = function(v) 
{
    len = length(v)
    v[2:len] - v[1:len-1]
}


progress = function(p) 
{
    dv = v.diff(p$visited)
    df = v.diff(p$frontier)
    data.frame(1:length(dv),dv,df)
}


makeplot = function(dir, basename, dataframe) 
{
	pngpath  = paste(dir, basename, ".png", sep="")
	
	print(pngpath)

	png(file=pngpath, width=1000, height=1000)
	plot(dataframe)
    abline(lm(dataframe[,2] ~ dataframe[,1]))
}


dir = "./"

makeplot(dir, "progress", prog)

graphics.off()
