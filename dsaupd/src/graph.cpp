#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>

#include "graph.h"


using namespace std;



GraphBase::GraphBase(int n, int edges, NormalizationType normalize, int verbosity) 
    : fNumNodes(n), 
      fMaxEdges(edges),
      fNumEdges(0),
      fNormalize(normalize),
      fEdgeConsts(0),
      fVerbosity(verbosity)
{ 
    fDegrees = new int[n];
    fEdges   = new int*[edges]; // [2]  
    
    for (int i = 0; i < n; ++i) 
        fDegrees[i] = 0; 
}

GraphBase::~GraphBase() 
{
    for (int i = 0; i < fNumEdges; ++i)
        delete fEdges[i];
    
    delete fEdges;
    delete fDegrees;
    delete fEdgeConsts; 
}


void GraphBase::addEdge(int i, int j)
{
    assert(fNumEdges < fMaxEdges);
    
    fEdges[fNumEdges] = new int[2];
    fEdges[fNumEdges][0] = i;
    fEdges[fNumEdges][1] = j;
    
    ++fNumEdges;
    ++fDegrees[i];
    ++fDegrees[j];
}


void GraphBase::addRandomEdge()
{
    double d = double(rand())/RAND_MAX;
    double n = fNumNodes;
    
    int i = int(n * d);
    
    d = double(rand())/RAND_MAX;
    
    int j = int((n-1) * d);

    if (j >= i)
    {
        ++j;
    }
    else // if (j < i)
    {
        int tmp = j;
        j = i;
        i = tmp;
    }
    addEdge(i,j);
}



void GraphBase::read(const string& filename)
{
    ifstream ifs(filename.c_str());
  
    if (!ifs)
        throw "failed to open edge list file";
      
    while (fNumEdges < fMaxEdges)
    {
        int i = -1, j = -1;
        
        if (!(ifs >> i)) break;
        if (!(ifs >> j)) break;
        if (i == -1)     break;
        if (j == -1)     break;
            
        if (i < j)
        {
            addEdge(i-1, j-1);
            
           // cout << i << ' ' << j << '\n';
        }
    }  
}


void GraphBase::save(const string& filename) const
{
    ofstream ofs(filename.c_str());
  
    if (!ofs)
        throw "failed to open file for writing";

    for (int e = 0; e < fNumEdges; ++e)
        ofs << fEdges[e][0] << '\t' << fEdges[e][1] << '\n';
}


void GraphBase::ensureEdgeConsts() const
{
    if (!fEdgeConsts)
        ((GraphBase *)this)->calcEdgeConsts();
}

void GraphBase::calcEdgeConsts()
{
    fEdgeConsts = new double[fNumEdges];
    
    for (int e = 0; e < fNumEdges; ++e) 
    {
        int i = fEdges[e][0];
        int j = fEdges[e][1];
        
        fEdgeConsts[e] = sqrt(fDegrees[i]*fDegrees[j]);
    }
}


// the laplacian version
void GraphBase::av(double *in, double *out) const
{
    switch (fNormalize) 
    {
        case SYM:
            {
                ensureEdgeConsts();
        
                for (int i = 0; i < fNumNodes; ++i) 
                    out[i] = (fDegrees[i] > 0) ? in[i] : 0;
              
                for (int e = 0; e < fNumEdges; ++e) 
                {
                    int i = fEdges[e][0];
                    int j = fEdges[e][1];
                      
                    out[i] -= in[j]/fEdgeConsts[e];
                    out[j] -= in[i]/fEdgeConsts[e];
                }
            }
            break;

        case RW:
            {
                for (int i = 0; i < fNumNodes; ++i) 
                    out[i] = (fDegrees[i] > 0) ? in[i] : 0;
              
                for (int e = 0; e < fNumEdges; ++e) 
                {
                    int i = fEdges[e][0];
                    int j = fEdges[e][1];
                      
                    out[i] -= in[j]/fDegrees[i];
                    out[j] -= in[i]/fDegrees[j];
                }
            }
            
            break;
            
        case NONE:
        default:
            {
                for (int i = 0; i < fNumNodes; ++i) 
                    out[i] = fDegrees[i] * in[i];
              
                for (int e = 0; e < fNumEdges; ++e) 
                {
                    int i = fEdges[e][0];
                    int j = fEdges[e][1];
                      
                    out[i] -= in[j];
                    out[j] -= in[i];
                }
            }
            break;
    }
    cout << '.' << flush;
}


void GraphBase::print() const
{
    for (int i = 0; i < fNumNodes; ++i) 
        cout << "degree(" << i << "," << fDegrees[i] << ")\n";
  
    for (int e = 0; e < fNumEdges; ++e) 
    {
        int i = fEdges[e][0];
        int j = fEdges[e][1];
          
        cout << "edge(" << i << "," << j << ")\n";
    }
}

/**
        Q = sum(e[i] - a[i]^2)
             i
                     
            where
            
        e[i] = (fraction of edges that join vertices in community i) / 2m
        
        a[i] = sum(k[v])/2m 
                    where v is in community i, 
                      and k is the degree of v 
                      
                      
    To test whether a particular division is meaningful we define a quality 
    function or "modularity" Q as follows [6]. 
    
    Let e[i,j] be the fraction of edges in the network that connect vertices 
    in group i to those in group j, and let 
    
        a[i] = sum(e[i,j])
                j
              
    Then

        Q = sum(e[i,i] - a[i]^2)
             i
        
    is the fraction of edges that fall within communities, minus the 
    expected value of the same quantity if edges fall at random without 
    regard for the community structure.
        
            "Fast algorithm for detecting community structure in networks"

             M. E. J. Newman, 2003

 **/
double GraphBase::modularity(int numCommunities, const int *assignments) const
{
    double _2m = 2*fNumEdges;
    
    double aa[numCommunities];
    double ee[numCommunities];

    cout << "\t2m = " << _2m << endl;
    
//    double *aa = new double[numCommunities];
//    double *ee = new double[numCommunities];
    
    for (int i = 0; i < numCommunities; ++i)
    {
        aa[i] = 0;
        ee[i] = 0;
    }

    for (int n = 0; n < fNumNodes; ++n)
    {
        int i = assignments[n];
        
        aa[i] += fDegrees[n];
        
        if (fVerbosity > 1)
            cout << "aa[" << i << "] = " << aa[i] << " (" << n << ')' << endl;
    } 
    
    for (int e = 0; e < fNumEdges; ++e) 
    {
        int ci = assignments[fEdges[e][0]]; // community ci
        int cj = assignments[fEdges[e][1]]; // community cj
        
        if (ci == cj)
            ee[ci] += 2;

        if (fVerbosity > 1)
        {
            cout << "ee[" << ci << "] = " << ee[ci] << '\t'
                 << " (" << fEdges[e][0] << ',' << fEdges[e][1] << ')' << endl; 
        }
    }

    cout << "\taa =";
    for (int i = 0; i < numCommunities; ++i)
        cout << ' ' << aa[i];
    cout << endl;

    cout << "\tee =";
    for (int i = 0; i < numCommunities; ++i)
        cout << ' ' << ee[i];
    cout << endl;

    cout << "\ta =";
    for (int i = 0; i < numCommunities; ++i)
        cout << ' ' << double(aa[i])/_2m;
    cout << endl;

    cout << "\te =";
    for (int i = 0; i < numCommunities; ++i)
        cout << ' ' << double(ee[i])/_2m;
    cout << endl;
    
    double mod = 0;

    for (int i = 0; i < numCommunities; ++i)
    {
        mod += ee[i];
        mod -= double(aa[i])/_2m*aa[i];
    }
    mod /= _2m;
    
//    int withinCounts[k];
//    int totalCounts[k];
//    
//    for (int i = 0; i < k; ++i)
//    {
//        withinCounts[i] = 0;
//        totalCounts[i]  = 0;
//    }
//        
//    for (int e = 0; e < fNumEdges; ++e) 
//    {
//        int ci = assignments[fEdges[e][0]]; // cluster ci
//        int cj = assignments[fEdges[e][1]]; // cluster cj
//        
//        if (ci == cj)
//        {
//            withinCounts[ci]++;
//            withinCounts[cj]++;
//        }
//        totalCounts[ci]++;
//        totalCounts[cj]++;
//    }
//    
//    double mod = 0;
//    
//    for (int i = 0; i < k; ++i)
//    {
//        mod += (withinCounts[i] - totalCounts[i]*totalCounts[i]/_2m);
//    }
//    mod /= _2m;
    
//    delete ee;
//    delete aa;
    
    return mod;
}


SubGraph::SubGraph(const GraphBase& base, const EigenBits& selection)
    : fBase(base),
      fSelection(selection)
{
}


void SubGraph::av(double *in, double *out) const
{
    if (fBase.fNormalize == GraphBase::SYM)
    {
        fBase.ensureEdgeConsts();

//        for (int i = 0; i < fNumNodes; ++i) 
//            out[i] = (fDegrees[i] > 0) ? in[i] : 0;
//      
//        for (int e = 0; e < fNumEdges; ++e) 
//        {
//            int i = fEdges[e][0];
//            int j = fEdges[e][1];
//              
//            out[i] -= in[j]/fEdgeConsts[e];
//            out[j] -= in[i]/fEdgeConsts[e];
//        }
    }
    else
    {
//        for (int i = 0; i < fNumNodes; ++i) 
//            out[i] = fDegrees[i] * in[i];
//      
//        for (int e = 0; e < fNumEdges; ++e) 
//        {
//            int i = fEdges[e][0];
//            int j = fEdges[e][1];
//              
//            out[i] -= in[j];
//            out[j] -= in[i];
//        }
    }
}


void SubGraph::print() const
{
    fBase.print();
}











CompleteGraph::CompleteGraph(int n)
    : GraphBase(n, n*(n-1)/2)
{
    for (int i = 0; i < n - 1; ++i)
        for (int j = i+1; j < n; ++j)
            addEdge(i, j);
}


StarGraph::StarGraph(int n)
    : GraphBase(n, n)
{
    for (int j = 1; j < n; ++j)
        addEdge(0, j);
}



RandomGraph::RandomGraph(int n, int edges)
    : GraphBase(n, edges)
{
    for (int e = 0; e < edges; ++e)
        addRandomEdge();
}


RandomCliquishGraph::RandomCliquishGraph(int n, int edges, int ncomm, int pctRandom)
    : GraphBase(n, edges)
{
    int randomEdges = edges * pctRandom / 100;  // 10% of edges are completely random
    int withinEdges = edges - randomEdges;
    
    for (int e = 0; e < withinEdges; ++e)
    {
        double d = double(rand())/RAND_MAX;
        
        int i = int(n/ncomm * d);
        
        d = double(rand())/RAND_MAX;
        
        int j = int((n-1)/ncomm * d);
        
        if (j >= i)
        {
            ++j;
        }
        else // if (j < i)
        {
            int tmp = j;
            j = i;
            i = tmp;
        }

        int which  = int(double(rand())/RAND_MAX * ncomm);
        int offset = int(double(which) * n / ncomm);     
        
        i += offset;
        j += offset;
        
        addEdge(i,j);
    }
    
    for (int e = 0; e < randomEdges; ++e)
        addRandomEdge();
}
