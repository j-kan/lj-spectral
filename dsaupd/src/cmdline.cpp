#include <string>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "dsaupd.h"
#include "graph.h"

namespace po = boost::program_options;

using namespace std;


static void graphFromFile(int dim, int numedges, int numvalues, 
                         const string& strNormalize,
                         const string& inputFile,
                         const string& outputValuesFile,
                         const string& outputVectorsFile)
{
    const int     n    = dim;           // The order of the matrix
    const int     tlen = numedges;      // << n*n
    const int     nev  = numvalues;     // The number of values to calculate
    
    GraphBase::NormalizationType normalize = GraphBase::NONE;
    
    if (strNormalize == "SYM")
        normalize = GraphBase::SYM;
    else if (strNormalize == "RW")
        normalize = GraphBase::RW;
    
    GraphBase                               g(n, tlen, normalize);
    EigenPairs<n, nev>                      eigen;
    Dsaupd<EigenPairs<n, nev>, TestMatrix>  solver(eigen);

    DsaupdVec   eigen(n, nev);

    g.read(inputFile);
    eigen.dsaupd(g);
    eigen.saveValues(outputValuesFile);
    eigen.saveVectors(outputVectorsFile);
}


int main(int argc, char **argv) 
{
    try 
    {
        int     k;
        int     n;
        int     edges;
        
        po::options_description desc("Allowed options");
        
        desc.add_options()
            ("help",            "produce help message")
            ("input,i",         po::value<string>(),    "input (edgelist) file")
            ("eigenvalues,l",   po::value<string>(),    "output eigenvalues file")
            ("eigenvectors,v",  po::value<string>(),    "output eigenvectors file")
            ("numvalues,k",     po::value<int>(&k)->default_value(10),       
                                                        "number of values/vectors to find")
            ("numnodes,n",      po::value<int>(&n)->default_value(10),    
                                                        "number of nodes in the graph")
            ("numedges,e",      po::value<int>(&edges)->default_value(10),    
                                                        "number of edges in the graph")
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
            n, edges, k, 
            vm["normalize"   ].as<string>(), 
            vm["input"       ].as<string>(), 
            vm["eigenvalues" ].as<string>(), 
            vm["eigenvectors"].as<string>()
        ); 
    }
    catch (exception& e) 
    {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...) 
    {
        cerr << "Exception of unknown type!\n";
    }

    return 0;
}


