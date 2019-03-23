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

const int     n     = 297987;   // The order of the matrix
const int     edges = 6263031;  // The number of edges
const int     nev   = 8;        // The number of values to calculate
const int     iter  = 1000;     // The max number of iterations 


typedef KMeans<n, nev>      Clusterer;
typedef AssignmentVec<n>    Assign;


#include "cluster.h"


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
        ("normalize",       po::value<string>()->default_value("SYM"),       
                                                    "type of normalization: none, rw, sym")
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
    string strNormalize = vm["normalize"].as<string>();
    
    cout << "======> graph file:    " << graphFile    << '\n'
         << "        data file:     " << dataFile     << '\n'
         << "        output dir:    " << outputDir    << '\n'
         << "        normalize:     " << strNormalize << '\n'
         << "        num restarts:  " << numRestarts  << '\n' << flush;

    GraphBase           *g = new GraphBase(n, edges);
    EigenPairs<n, nev>  *e = new EigenPairs<n, nev>();
    
    e->readVectors(dataFile);
    
    cout << "        read eigenvectors" << endl;
    
    g->read(graphFile);

    cout << "        read graph" << endl;
    
    cluster(*g, e->vectors(), outputDir, numRestarts, strNormalize == "SYM" ? 1 : 0);
              
    delete g;
    delete e;
}
