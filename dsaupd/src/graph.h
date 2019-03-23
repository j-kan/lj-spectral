#ifndef GRAPH_H_
#define GRAPH_H_

#include "dsaupd.h"

#include <boost/dynamic_bitset.hpp>

typedef boost::dynamic_bitset<> EigenBits;

template <int dim, int num_values = 2>
class EigenPartition
{
    double      fValues[num_values];
    EigenBits   fFiedler;

public:

    EigenPartition() 
        : fFiedler(dim)
    {
        for (int j = 0; j < num_values; ++j)
            fValues[j] = 0;
    }
    
    int  numValues() const  { return num_values; }
    int  n() const          { return dim;        }
    bool hasVectors() const { return true;       }    
    
    void   setValue(int i, double value)   { fValues[i] = value; }   
    double getValue(int i) const           { return fValues[i];  }   

    void   setVectorComponent(int i, int j, double value)  { if (i == 1) fFiedler[j] = (value > 0); }   
    
    void   print() const                   { printValues(); printVectors(); }
    void   printValues() const;
    void   printVectors() const;
    
    int    count() const                   { return fFiedler.count(); }
};


template <int dim, int num_values>
void EigenPartition<dim, num_values>::printValues() const
{    
    cout << "\neigenvalues = c(";
    for (int i = 0; i < num_values; ++i)
    { 
        if (i > 0)
            cout << ", ";
        
        cout << fValues[i];
    }
    cout << ")\n";
}


template <int dim, int num_values>
void EigenPartition<dim, num_values>::printVectors() const
{
    cout << "\nfiedler = c(";
    
    for (int j = 0; j < dim; ++j)
    {
        cout << fFiedler[j];
        
        if (j < dim-1)
            cout << ", ";
    }
    cout << ")\n";
}





class Graph : public ArpackCallback
{
public:
    virtual int numNodes() const = 0;
};


class GraphBase : public Graph
{
    friend class SubGraph;
    
public:

    enum NormalizationType {
        NONE = 0,
        RW,
        SYM,
        NUM_TYPES
    };
    
    int                 **fEdges;         // T[tlen][2]
    int                  *fDegrees;       // [n]
    int                   fNumNodes;
    int                   fNumEdges;
    int                   fMaxEdges;
    
    NormalizationType     fNormalize;
    double               *fEdgeConsts;

    int                   fVerbosity;
    
public:

            GraphBase(int n, int edges, NormalizationType normalize = SYM, int verbosity = 1);
            ~GraphBase();
        
            void read(const string& filename);
            void addEdge(int i, int j);
            void addRandomEdge();
            
    virtual void print() const;
    virtual void save(const string& filename) const;
        
    virtual int  n() const        { return fNumNodes; }
    virtual int  numNodes() const { return fNumNodes; }
    virtual void av(double *in, double *out) const;
    
            void setVerbosity(int v) { fVerbosity = v; }
    
            double modularity(int k, const int *assignments) const;
            
            
            void setNormalization(const string& strNormalize)
            {
                if (strNormalize == "SYM")
                    fNormalize = GraphBase::SYM;
                else if (strNormalize == "RW")
                    fNormalize = GraphBase::RW;
                else
                    fNormalize = GraphBase::NONE;
            }
    
private:

            void ensureEdgeConsts() const;
            void calcEdgeConsts();
};


class SubGraph : public Graph
{
    const GraphBase& fBase;
    const EigenBits& fSelection;
    
public:
                 SubGraph(const GraphBase& base, const EigenBits& selection);

    virtual void av(double *in, double *out) const;
    virtual void print() const;
    virtual int  numNodes() const { return (int)fSelection.count(); }
};


class CompleteGraph : public GraphBase
{
public:
        CompleteGraph(int n);
};


class StarGraph : public GraphBase
{
public:
        StarGraph(int n);
};


class RandomGraph : public GraphBase
{
public:
        RandomGraph(int n, int edges);
};

class RandomCliquishGraph : public GraphBase
{
public:
        RandomCliquishGraph(int n, int edges, int ncomm = 2, int pctRandom = 10);
};



#endif /*GRAPH_H_*/
