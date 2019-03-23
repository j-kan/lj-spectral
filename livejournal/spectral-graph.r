make.indicator <- function(v)
{ 
    sign(as.real(v)) + 2 
}


make.graph = function(adjFileName)
{
    adj = read.table(adjFileName)
    graph.adjacency(adj, mode="upper")
}

make
colors=c("green","white","blue")


g50 = make.graph("undirected-adjacency-matrix.tab")
e50 = eigen(graph.laplacian(g50))


color.graph = function(g, eig, i, c = colors)
{
   set.vertex.attribute(g, "color", value=colors[make.indicator(eig$vectors[,i])])
}


graph.cut.plot = function(g, norm, layout=layout.fruchterman.reingold)
{
	make.indicator <- function(v)
	{ 
	    sign(as.real(v)) + 2 
	}
	lap = graph.laplacian(g, normalized = norm)
	eig = eigen(lap)
	i   = dim(lap)[1] - 1
	colors = rainbow(3)
    g2  = set.vertex.attribute(g, "color", value=colors[make.indicator(eig$vectors[,i])])
	plot(g2, layout=layout, vertex.size=10, vertex.color="a:color")
}


laplacian.kmeans.plot = function(g, lap, k, layout=layout.fruchterman.reingold)
{
	eig = eigen(lap)
	i   = dim(lap)[1] - 1
	m   = cbind(eig$vectors[,i], eig$vectors[,i-1], eig$vectors[,i-2])
	km  = kmeans(m, k)
	colors = rainbow(k)
    g2 = set.vertex.attribute(g, "color", value=colors[km$cluster])
	plot(g2, layout=layout, vertex.size=10, vertex.color="a:color")
}

graph.kmeans.plot = function(g, k, norm = TRUE, layout=layout.fruchterman.reingold)
{
	lap = graph.laplacian(g, normalized = norm)
	laplacian.kmeans.plot(g, lap, k, layout)
}


graph.color.plot <- function(g, assignments, layout=layout.fruchterman.reingold)
{
	k <- max(assignments)+1
	colors <- rainbow(k)
	V(g)$color = colors[assignments+1]
	g$layout = layout(g)	
	plot(g, vertex.size=4, labels=NA)
	g
}

#    g2 <- set.vertex.attribute(g, "color", value=colors[assignments])
#    g2 <- set.graph.attribute(g2, "layout", layout(g))


gg50 = color.graph(g50, e50, 49, colors)

plot(gg50, layout=layout.lgl, vertex.size=10, vertex.color="a:color")

