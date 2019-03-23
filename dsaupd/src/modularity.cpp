#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "assignments.h"
#include "graph.h"


using namespace std;

const int     n     = 297987;   // The order of the matrix
const int     edges = 2616134;  // The number of edges
const int     nev   = 100;      // The number of values to calculate
const int     k     = 50;       // The number of communities

int main(int argc, char **argv) 
{
    GraphBase *g = new GraphBase(n, edges);
    
    g->read(argv[1]);
    g->setVerbosity(2);

    cout << "graph: " << argv[1] << endl;
    
    for (int i = 2; i < argc; ++i)
    {
        AssignmentVec<n> assignments;
        
        assignments.read(argv[i]);
        
        cout << "assignments: " << argv[i] << endl;
    
        double modularity = g->modularity(k, assignments.asArray());
        
        cout << "modularity = " << modularity << '\n';
    }
}
