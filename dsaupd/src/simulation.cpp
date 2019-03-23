#include <iostream>
#include <fstream>
#include <sstream>

//#include "boost/filesystem.hpp"   

#include "dsaupd.h"
#include "eigen.h"
#include "graph.h"
#include "kmeans.h"

using namespace std;

const int     n     = 1000;   // The order of the matrix
const int     edges = n * 10; // The number of edges
const int     nev   = 10;     // The number of values to calculate
const int     ncomm = 100;    // The number of artificially-induced communities
//const int     k     = 100;    // The number of clusters

//const int     n     = 10;    // The order of the matrix
//const int     edges = n * 3; // The number of edges
//const int     nev   = 3;     // The number of values to calculate
//const int     ncomm = 3;     // The number of artificially-induced communities
//const int     k     = 3;     // The number of clusters
const int     iter  = 100;   // The max number of iterations 


typedef EigenPairs<n, nev>                 Eigen;
typedef KMeans<n, nev>                     Clusterer;
typedef Dsaupd<Eigen, GraphBase>           Solver;
typedef Clusterer::Assignments             Assign;


static void calcEigen(GraphBase& g, bool normalize, int numKmeansRestarts, const string& dir)
{
    Eigen   eigen;
    Solver  solver(eigen);

    solver.dsaupd(g);
    eigen.printValues();

    //boost::filesystem::create_directory(dir.c_str());

    string file1 = dir + "/eigenvalues.txt";
    string file2 = dir + "/eigenvectors.txt";
    eigen.saveValues(file1);
    eigen.saveVectors(file2);

    cout << "======> kmeans\n" << flush; 
    
    for (int k = 50; k <= 200; k += 50)  // number of clusters
    {
        double q[numKmeansRestarts];
        double maxQ = -1;
        Assign bestAssignments;
    
        for (int i = 0; i < numKmeansRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means[" << k << "] " << i << "\n\n" << flush;
            
            Clusterer  kmeans(eigen.vectors(), k, normalize);
            
            kmeans.run(iter);
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
        {
            cout << "\t" <<  q[i];

            if (q[i] == maxQ)
                cout << " <==";
                
            cout << endl;
        }
        bestAssignments.print();
    }
}

static void calcPartition(GraphBase& g)
{
    EigenPartition<n>                    eigen;
    Dsaupd<EigenPartition<n>, GraphBase> solver(eigen);

    solver.dsaupd(g);
    eigen.print();
    
    cout << "||partition " << 1 << "|| = " << eigen.count() << '\n';
}

static void randomCommunityGraph()
{
    stringstream graphFileName;
    
    graphFileName << "edgelist-random-" << n << "-nodes-" << edges << "-edges-" << ncomm << "-communities.txt";
    
    cout << "======> random community graph: " 
         << graphFileName.str() << '\n';
    
    cout << "======> symmetric normalization\n"; 

    RandomCliquishGraph     g(n, edges, ncomm);
    g.save(graphFileName.str());

    calcPartition(g);
    calcEigen(g, true, 10, "random-100-communities-Lsym");

    cout << "======> random-walk normalization\n"; 

    g.fNormalize = GraphBase::RW;
    
    calcPartition(g);
    calcEigen(g, false, 10, "random-100-communities-Lrw"); //false);
}



int main()
{
    srand(90210);
    
    randomCommunityGraph();
}
