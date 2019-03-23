#include <iostream>
#include <fstream>
#include "dsaupd.h"
#include "graph.h"
#include "eigen.h"
#include "kmeans.h"

//#include <boost/numeric/ublas/matrix_sparse.hpp>
//#include <boost/numeric/ublas/io.hpp>

using namespace std;
//using namespace boost::numeric::ublas;



static void printModularity(const GraphBase& g, int k, const int *assignments)
{
    cout << "=====> assignments = ";

    for (int i = 0; i < g.n(); ++i)
        cout << assignments[i] << ' ';
    
    cout << '\n';
    cout << "       modularity: " << g.modularity(k, assignments) << '\n';
}


static void testModularity(const GraphBase& g)
{
    const int k = 2;
    const int n = g.n();
    
    int *assignments = new int[n];
    
    for (int i = 0; i < n; ++i)
        assignments[i] = int(double(rand())/RAND_MAX * k);

    printModularity(g, k, assignments);
    
    for (int i = 0; i < n; ++i)
        assignments[i] = i*k/n;

    printModularity(g, k, assignments);

    for (int i = 0; i < n; ++i)
        assignments[i] = 0;

    printModularity(g, k, assignments);

    assignments[n-1] = 1;

    printModularity(g, k, assignments);

    for (int i = n/2; i < n; ++i)
        assignments[i] = i - n/2 + 1;

    printModularity(g, n/2, assignments);
    
    delete assignments;
}

static void graphFromFile()
{
    const int     n    = 100;   // The order of the matrix
    const int     tlen = 1528;  // << n*n
    const int     nev  = 99;     // The number of values to calculate
    
    GraphBase                               g(n, tlen);
    EigenPairs<n, nev>                      eigen;
    Dsaupd<EigenPairs<n, nev>, GraphBase>   solver(eigen);

    g.read("input/test100Edgelist.txt");
    solver.dsaupd(g);
    eigen.print();
    
    testModularity(g);
}


static void completeGraph()
{
    const int     n    = 10;  // The order of the matrix
    const int     nev  = 9;   // The number of values to calculate

    cout << "======> complete graph: " << n << '\n';
    
    CompleteGraph                               g(n);
    EigenPairs<n, nev>                          eigen;
    Dsaupd<EigenPairs<n, nev>, CompleteGraph>   solver(eigen);

    //g.print();
    g.save("complete-graph/complete-graph.pairs");

    solver.dsaupd(g);
    eigen.printValues();
    
    eigen.saveValues( "complete-graph/eigenvalues.txt");
    eigen.saveVectors("complete-graph/eigenvectors.txt");
    
    testModularity(g);
}


static void starGraph()
{
    const int     n    = 10;    // The order of the matrix
    const int     nev  = 9;     // The number of values to calculate
    
    cout << "======> star graph: " << n << '\n';
    
    StarGraph                               g(n);
    EigenPairs<n, nev>                      eigen;
    Dsaupd<EigenPairs<n, nev>, StarGraph>   solver(eigen);

    //g.print();
    g.save("star-graph/star-graph.pairs");

    solver.dsaupd(g);
    eigen.printValues();
    
    eigen.saveValues( "star-graph/eigenvalues.txt");
    eigen.saveVectors("star-graph/eigenvectors.txt");
    
    testModularity(g);
}

static void randomGraph()
{
    const int     n     = 1000;    // The order of the matrix
    const int     edges = n * 50;  // The number of edges
    const int     nev   = 3;       // The number of values to calculate
    
    cout << "======> random graph: " << n << ", " << edges << '\n';
    
    RandomGraph                             g(n, edges);
    
    g.save("random-graph/random-graph.pairs");
    
    {
        EigenPairs<n, nev>                      eigen;
        Dsaupd<EigenPairs<n, nev>, RandomGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
        
        eigen.saveValues( "random-graph/eigenvalues.txt");
        eigen.saveVectors("random-graph/eigenvectors.txt");
    }
        
    testModularity(g);
}

static void randomGraphWith2Communities()
{
    const int     n     = 100;    // The order of the matrix
    const int     edges = n * 5;  // The number of edges
    const int     nev   = 3;      // The number of values to calculate
    const int     k     = 2;      // The number of clusters
    
    const int     maxKmeansIterations = 1000;
    const int     numKmeansRestarts   = 10;

    cout << "======> random graph with 2 communities: " << n << ", " << edges << '\n';
    
    RandomCliquishGraph     g(n, edges, k, 1);  // 1% cross-cluster edges
    // g.print();

    g.save("random-2-communities/graph.pairs");

    {
        EigenPairs<n, nev>                              eigen;
        Dsaupd<EigenPairs<n, nev>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
        
        eigen.saveValues( "random-2-communities/Lsym-eigenvalues.txt");
        eigen.saveVectors("random-2-communities/Lsym-eigenvectors.txt");
        
        double q[10];
        
        for (int i = 0; i < numKmeansRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means " << i << "\n\n";
            
            KMeans<n, nev>  kmeans(eigen.vectors(), k);
            
            kmeans.run(maxKmeansIterations);
            kmeans.print();
    
            q[i] = g.modularity(k, kmeans.assignmentArray());
            
            cout << "=====> modularity: " << q[i] << '\n';
        }
            
        for (int i = 0; i < 10; ++i)
        { 
            cout << "\t" <<  q[i] << '\n';
        }
    }
    
    g.fNormalize = GraphBase::RW;
    
    {
        EigenPairs<n, nev>                              eigen;
        Dsaupd<EigenPairs<n, nev>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
        
        eigen.saveValues( "random-2-communities/Lrw-eigenvalues.txt");
        eigen.saveVectors("random-2-communities/Lrw-eigenvectors.txt");

        double q[10];
        
        for (int i = 0; i < numKmeansRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means " << i << "\n\n";
            
            KMeans<n, nev>  kmeans(eigen.vectors(), k);
            
            kmeans.run(maxKmeansIterations);
            kmeans.print();
    
            q[i] = g.modularity(k, kmeans.assignmentArray());
            
            cout << "=====> modularity: " << q[i] << '\n';
        }
            
        for (int i = 0; i < 10; ++i)
        { 
            cout << "\t" <<  q[i] << '\n';
        }
    }
    
    {
        EigenPartition<n>                              eigen;
        Dsaupd<EigenPartition<n>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.print();
        
        cout << "||partition 1|| = " << eigen.count() << '\n';
    }
    
    
    testModularity(g);
}



static void randomGraphWith3Communities()
{
    const int     n     = 100;    // The order of the matrix
    const int     edges = n * 5;  // The number of edges
    const int     ncomm = 3;      // The number of artificially-induced communities

    cout << "======> random community graph: " 
         << n << ", " << edges << ", " << ncomm << '\n';
    
    RandomCliquishGraph     g(n, edges, ncomm);
    
    g.save("random-3-communities/graph.pairs");
    //g.print();

    {
        EigenPartition<n>                              eigen;
        Dsaupd<EigenPartition<n>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.print();
	    
	    cout << "||partition 1|| = " << eigen.count() << '\n';
    }
    
    const int     nev   = 10;     // The number of values to calculate
    const int     k     = 3;      // The number of clusters

    const int     maxKmeansIterations = 1000;
    const int     numKmeansRestarts   = 10;
    
    double q_sym[numKmeansRestarts];
    double q_rw[numKmeansRestarts];

    {
        EigenPairs<n, nev>                              eigen;
        Dsaupd<EigenPairs<n, nev>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
        
        eigen.saveValues( "random-3-communities/Lsym-10-eigenvalues.txt");
        eigen.saveVectors("random-3-communities/Lsym-10-eigenvectors.txt");

        for (int i = 0; i < numKmeansRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means " << i << "\n\n";
            
    	    KMeans<n, nev>  kmeans(eigen.vectors(), k);
    	    
    	    kmeans.run(maxKmeansIterations);
    	    kmeans.print();
    
            q_sym[i] = g.modularity(k, kmeans.assignmentArray());
            
            cout << "=====> modularity: " << q_sym[i] << '\n';
        }
	}

    g.fNormalize = GraphBase::RW;
    
    {
        EigenPairs<n, nev>                              eigen;
        Dsaupd<EigenPairs<n, nev>, RandomCliquishGraph> solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
        
        eigen.saveValues( "random-3-communities/Lrw-10-eigenvalues.txt");
        eigen.saveVectors("random-3-communities/Lrw-10-eigenvectors.txt");

        double q[10];
        
        for (int i = 0; i < numKmeansRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means " << i << "\n\n";
            
            KMeans<n, nev>  kmeans(eigen.vectors(), k);
            
            kmeans.run(maxKmeansIterations);
            kmeans.print();
    
            q_rw[i] = g.modularity(k, kmeans.assignmentArray());
            
            cout << "=====> modularity: " << q_rw[i] << '\n';
        }
    }
        
    cout << "\n\n---------------------- K-means results: Lsym\n";
        
    for (int i = 0; i < numKmeansRestarts; ++i)
        cout << "\t" <<  q_sym[i] << '\n';

    cout << "\n\n---------------------- K-means results: Lrw\n";
        
    for (int i = 0; i < numKmeansRestarts; ++i)
        cout << "\t" <<  q_rw[i] << '\n';

    {        
        typedef EigenPairs<n, nev>                 Eigen;
        typedef KMeans<n, nev>                     Clusterer;
        typedef Dsaupd<Eigen, GraphBase>           Solver;
        typedef Clusterer::Assignments             Assign;

        Eigen   eigen;
        Solver  solver(eigen);
    
        solver.dsaupd(g);
        eigen.printValues();
    
        //boost::filesystem::create_directory(dir.c_str());
        
        eigen.saveValues( "random-3-communities/eigenvalues.txt");
        eigen.saveVectors("random-3-communities/eigenvectors.txt");
    
        for (int k = 2; k <= 5; k += 1)  // number of clusters
        {
            double q[numKmeansRestarts];
            double maxQ = -1;
            Assign bestAssignments;
        
            for (int i = 0; i < numKmeansRestarts; ++i)
            { 
                cout << "\n\n---------------------- K-means " << i << "\n\n" << flush;
                
                Clusterer  kmeans(eigen.vectors(), k, false);
                
                kmeans.run(maxKmeansIterations);
                kmeans.print();
        
                q[i] = g.modularity(k, kmeans.assignmentArray());
                
                cout << "=====> modularity: " << q[i] << '\n' << flush;
                
                if (q[i] > maxQ)
                {
                    maxQ = q[i];
                    bestAssignments = kmeans.assignments(); 
                }                
            }
        
            cout << "\n\n---------------------- K-means results:\n";
        
            for (int i = 0; i < numKmeansRestarts; ++i)
                cout << "\t" <<  q[i] << '\n';
        }
    }    
}



int main()
{
    //srand (time (0));

    srand(90210);
    
   // graphFromFile();
    completeGraph();
    starGraph();
    randomGraph();
    randomGraphWith2Communities();
    randomGraphWith3Communities();
    //randomCliquishGraphKMeans();
    
//    cout << "sizeof(int) "  << sizeof(int)  << '\n';
//    cout << "sizeof(long) " << sizeof(long) << '\n';
}



