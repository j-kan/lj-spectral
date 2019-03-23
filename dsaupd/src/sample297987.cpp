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
const int     nev   = 32;       // The number of values to calculate
const int     iter  = 1000;     // The max number of iterations 


typedef EigenPairs<n, nev>              Eigen;
typedef Dsaupd<Eigen, GraphBase>        Solver;

typedef KMeans<n, nev>      Clusterer;
typedef AssignmentVec<n>    Assign;


#include "cluster.h"


static void calcEigen(GraphBase& g, bool normalize, int numKmeansRestarts, const string& dir)
{
    Eigen   *eigen  = new Eigen();
    Solver  *solver = new Solver(*eigen);

    solver->dsaupd(g);
    eigen->printValues();

    //boost::filesystem::create_directory(dir.c_str());

    string file1 = dir + "/eigenvalues.txt";
    string file2 = dir + "/eigenvectors.txt";
    eigen->saveValues(file1);
    eigen->saveVectors(file2);

    cout << "======> kmeans\n" << flush; 
    
    cluster(g, eigen->vectors(), dir, numKmeansRestarts, normalize);

    delete solver;
    delete eigen;
}

static void calcPartition(GraphBase& g)
{
    EigenPartition<n>                    eigen;
    Dsaupd<EigenPartition<n>, GraphBase> solver(eigen);

    solver.dsaupd(g);
    eigen.print();
    
    cout << "||partition " << 1 << "|| = " << eigen.count() << '\n';
}

static void graphFromFile(const string& inputFile,
                          const string& outputDir,
                          const string& strNormalize,
                          int numRestarts)
{
    GraphBase::NormalizationType normalize = GraphBase::NONE;
    
    if (strNormalize == "SYM")
        normalize = GraphBase::SYM;
    else if (strNormalize == "RW")
        normalize = GraphBase::RW;
    
    cout << "======> input file:    " << inputFile    << '\n'
         << "        output dir:    " << outputDir    << '\n'
         << "        normalization: " << strNormalize << '\n'
         << "        num restarts:  " << numRestarts  << '\n';

    GraphBase *g = new GraphBase(n, edges, normalize);
    
    g->read(inputFile.c_str());

    calcEigen(*g, 
              normalize == GraphBase::SYM ? true : false, 
              numRestarts, 
              outputDir);
              
    delete g;
}


int main(int argc, char **argv) 
{
    srand(90210);
    
    int numRestarts;
    
    po::options_description desc("Allowed options");
    
    desc.add_options()
        ("help",            "produce help message")
        ("graph,g",         po::value<string>(),    "input (edgelist) file")
        ("outputdir,o",     po::value<string>(),    "output directory")
        ("numrestarts,r",   po::value<int>(&numRestarts)->default_value(10),       
                                                    "number of kmeans restarts")
        ("normalize",       po::value<string>(),       
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
    
    graphFromFile(
        vm["graph"       ].as<string>(), 
        vm["outputdir"   ].as<string>(), 
        vm["normalize"   ].as<string>(), 
        numRestarts
    ); 
}
