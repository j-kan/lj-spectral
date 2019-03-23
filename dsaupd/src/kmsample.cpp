#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/program_options.hpp>

//#include "boost/filesystem.hpp"   

#include "dsaupd.h"
#include "eigen.h"
#include "graph.h"
#include "kmeans.h"


namespace po = boost::program_options;

using namespace std;

//const string inputfile(  "is_friend_of_sample.pairs");
//const string eigenvalues("sample-129456/eigenvalues.txt");
//const string eigenvalues("sample-129456/eigenvectors.txt");

const int     n     = 129456;   // The order of the matrix
const int     edges = 2241044;  // The number of edges
const int     nev   = 100;      // The number of eigenvalues
const int     iter  = 1000;     // The max number of iterations 


typedef KMeans<n, nev>      Clusterer;
typedef AssignmentVec<n>    Assign;


static void cluster(const GraphBase& g, EigenPairs<n, nev>& eigen, const string& fOutputDir, int fKmeansNumRestarts, bool fKmeansNormalize = false, int fKmeansMaxIterations=1000)
{
    //boost::filesystem::create_directory(dir.c_str());

    for (int k = 20; k <= 320; k *= 2)  // number of clusters
    {
        double q[fKmeansNumRestarts];
        double maxQ = -1;
        Assign bestAssignments;
    
        for (int i = 0; i < fKmeansNumRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means[" << k << "] " << i << "\n\n" << flush;
            
            Clusterer  *kmeans = new Clusterer(eigen.vectors(), k, fKmeansNormalize);
            
            kmeans->run(fKmeansMaxIterations);
            kmeans->print();
    
            q[i] = g.modularity(k, kmeans->assignmentArray());
            
            cout << "=====> modularity: " << q[i] << '\n' << flush;
            
            if (q[i] > maxQ)
            {
                maxQ = q[i];
                bestAssignments = kmeans->assignments(); 
            }
            
            delete kmeans;                
        }
    
        cout << "\n\n---------------------- K-means results:\n";
    
        for (int i = 0; i < fKmeansNumRestarts; ++i)
        {
            cout << "\t" <<  q[i];

            if (q[i] == maxQ)
                cout << " <==";
                
            cout << endl;
        }
        
        stringstream path;
        
        path << fOutputDir << "/assignments-k-" << k << "-Q-" << maxQ << ".txt";
        
        bestAssignments.save(path.str());
    }
}


int main(int argc, char **argv) 
{
    srand(90210);
    
    int numRestarts;
    
    po::options_description desc("Allowed options");
    
    desc.add_options()
        ("help",            "produce help message")
        ("graph,g",         po::value<string>(),    "graph input (edgelist) file")
        ("data,d",          po::value<string>(),    "data input (eigenvector) file")
        ("outputdir,o",     po::value<string>(),    "output directory")
        ("numrestarts,r",   po::value<int>(&numRestarts)->default_value(10),       
                                                    "number of kmeans restarts")
    ;

    po::variables_map vm;
            
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) 
    {
        cout << desc << "\n";
        return 1;
    }
    
    string graphFile    = vm["graph"    ].as<string>();
    string dataFile     = vm["data"     ].as<string>();
    string outputDir    = vm["outputdir"].as<string>();
    
    cout << "======> graph file:    " << graphFile    << '\n'
         << "        data file:     " << dataFile     << '\n'
         << "        output dir:    " << outputDir    << '\n'
         << "        num restarts:  " << numRestarts  << '\n' << flush;

    GraphBase           *g = new GraphBase(n, edges);
    EigenPairs<n, nev>  *e = new EigenPairs<n, nev>();
    
    e->readVectors(dataFile);
    
    cout << "        read eigenvectors" << endl;
    
    g->read(graphFile);

    cout << "        read graph" << endl;
    
    cluster(*g, *e, outputDir, numRestarts);
              
    delete g;
    delete e;
}
