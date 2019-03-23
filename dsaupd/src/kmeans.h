#ifndef KMEANS_H_
#define KMEANS_H_

#include <iostream>
#include <cmath>
#include <limits>
#include <set>


#include "assignments.h"


using namespace std;


template <class Vector, int dim>    // here, dim == the dim of the eigenmatrix row
class Cluster
{
    double     fCentroid[dim];
    int        fSize;
    double     fScatter;
    
public:

    Cluster() { zero(); }
    
    void          zero()  
    { 
        fSize = 0; 
        fScatter = 0; 
        for (int i = 0; i < dim; ++i) 
            fCentroid[i] = 0;
    }
    
    void          setCentroid(const Vector& v)   { for (int i = 0; i < dim; ++i) { fCentroid[i] = v[i]; } }

    const Vector& centroid() const      { return fCentroid; }
    Vector&       centroid()            { return fCentroid; }
    
    int           size() const          { return fSize;     }
    
    void          addVector(const Vector& v)
    {
        ++fSize;
        for (int i = 0; i < dim; ++i) 
            fCentroid[i] += v[i];  
    }
    
    void          normalize()
    {
        if (fSize > 0)
            for (int i = 0; i < dim; ++i) 
                fCentroid[i] /= fSize;
    }
};


template <int numvectors, int dim>
class KMeans
{
public:

    typedef double                    Vector[dim];
    typedef Vector                    Vectors[numvectors];
    typedef AssignmentVec<numvectors> Assignments;

private:
    
    Vectors&                fVectors;
    int                     fK;
    Assignments             fAssignments;
    Cluster<Vector, dim>   *fClusters;

    int                     fMaxIterations;
    bool                    fPrintCentroids;
    short                   fVerbosity;
    
public:

                    KMeans(Vectors& vectors, int k, bool normalize = false)
                        : fVectors(vectors), fK(k), fPrintCentroids(0), fVerbosity(1)
                    { 
                        fClusters = new Cluster<Vector, dim>[k];
                        
                        if (normalize)
                        {
                            for (int i = 0; i < numvectors; ++i)
                            {
                                Vector& v    = fVectors[i];
                                double  norm = 0;
                                
                                for (int j = 0; j < dim; ++j)
                                    norm += v[j] * v[j];
                                
                                norm = sqrt(norm);
    
                                for (int j = 0; j < dim; ++j)
                                    v[j] /= norm;
                            }
                        }
                    }
                        
                    ~KMeans() { delete fClusters; }

              void   run(int maxIterations);      
              void   print() const; 
              void   printClusterSizes() const;
              
        const Assignments& assignments() const { return fAssignments; }
        const int *        assignmentArray() const 
                                               { return fAssignments.asArray(); }
              int    k() const                 { return fK;           }
              void   setPrintCentroids(bool p) { fPrintCentroids = p; }          

protected:

              void   initAssignments();
              void   initCentroids();
              bool   runOneIteration();
              double calcSquaredDistance(const Vector& v1, const Vector& v2) const;
              void   updateCentroids();
              
              int    randomVector() const;
              int    randomCluster() const;
              int    randomElement() const;
    
              ostream& printVector(ostream& ostr, const Vector& v) const
              {
                ostr << '[';
                
                for (int i = 0; i < dim; ++i) 
                {
                    if (i > 0)
                        ostr << ", ";
                    
                    ostr << v[i];
                }
                ostr << ']';
                
                return ostr;
              }
};

template <int numvectors, int dim>
void KMeans<numvectors, dim>::run(int maxIterations)
{
    initAssignments();
    initCentroids();
    
    for (int it = 0; it < maxIterations; ++it) 
    {
        if (fVerbosity > 0)
            cout << "    iteration " << it << '\t' << flush;
        
        bool changed = runOneIteration();
        
        if (!changed)
            break;
    }
}


template <int numvectors, int dim>
int KMeans<numvectors, dim>::randomVector() const
{
    double d = double(rand())/RAND_MAX;
    return int(numvectors * d);
}

template <int numvectors, int dim>
int KMeans<numvectors, dim>::randomCluster() const
{
    double d = double(rand())/RAND_MAX;
    return int(fK * d);
}

template <int numvectors, int dim>
int KMeans<numvectors, dim>::randomElement() const
{
    double d = double(rand())/RAND_MAX;
    return int(dim * d);
}

template <int numvectors, int dim>
void KMeans<numvectors, dim>::initAssignments()
{
    for (int i = 0; i < numvectors; ++i)
        fAssignments[i] = randomCluster();
}


template <int numvectors, int dim>
void KMeans<numvectors, dim>::initCentroids()
{
    typedef set<int,greater<int> > IntSet;
    
    IntSet alreadyPicked;

    int k = 0;
    
    // choose initial centroids to be orthogonal

    while (k < min(fK, dim))
    {
        int                          j      = randomElement();
        pair<IntSet::iterator, bool> status = alreadyPicked.insert(j);

        if (status.second)  // successful insert, i.e. not already there
        {
            Vector& centroid = fClusters[k].centroid();
            
            centroid[j] = 1;
            
            ++k;
        }
    }
    
    // choose the rest randomly from the vector set
    
    while (k < fK)
    {
        int i = randomVector();
        
//        if (fAssignments[i] == k)
//        {
            fClusters[k].setCentroid(fVectors[i]);
            ++k;
//        } 
    }
}


template <int numvectors, int dim>
bool KMeans<numvectors, dim>::runOneIteration()
{
    bool changed = false;
    
    for (int i = 0; i < numvectors; ++i) 
    {
        double minDistance = numeric_limits<double>::infinity();
        int    minK        = -1;
        
        for (int k = 0; k < fK; ++k) 
        {
//            double distance = calcSquaredDistance(fVectors[i], fClusters[k].centroid());

            const Vector& v1       = fVectors[i];
            const Vector& v2       = fClusters[k].centroid();
            double        distance = 0; // squared distance, actually
            
            for (int j = 0; j < dim; ++j) 
            {
                double diff = v1[j] - v2[j];
                
                distance += diff * diff;
//                
//                if (distance > minDistance) // not interesting...
//                    break;
            }
            
            if (distance < minDistance)
            {
                minDistance = distance;
                minK        = k;
            }
        }
        
        if (fAssignments[i] != minK)
        {
//            cout << "\tvector " << i << " = ";
//            printVector(cout, fVectors[i]) << " was in " << fAssignments[i] << ", now in " << minK << '\n' << flush;

            if (fVerbosity > 1)
            cout << " | v[" << i << "]: " << fAssignments[i] << " => " << minK << flush;
            else if (fVerbosity > 0)
                cout << '.'; // << flush;
            
            changed = true;
            fAssignments[i] = minK;
        }
    }
    cout << endl;
    
    if (changed)
        updateCentroids();
        
    return changed;
}


template <int numvectors, int dim>
double KMeans<numvectors, dim>::calcSquaredDistance(const Vector& v1, const Vector& v2) const
{
    double sum = 0;
    
    for (int i = 0; i < dim; ++i) 
    {
        double diff = v1[i] - v2[i];
        
        sum += diff * diff;
    }
    return sum;
}


template <int numvectors, int dim>
void KMeans<numvectors, dim>::updateCentroids()
{
    for (int k = 0; k < fK; ++k) 
        fClusters[k].zero();
    
    for (int i = 0; i < numvectors; ++i) 
    {
        int k = fAssignments[i];
        
        fClusters[k].addVector(fVectors[i]);
    }

    for (int k = 0; k < fK; ++k) 
    {
        fClusters[k].normalize();
        
        if (fPrintCentroids)
        {
	        cout << "\tcentroid " << k << " = ";
	        printVector(cout, fClusters[k].centroid()) << '\n' << flush;
        }
    }    
    
}



template <int numvectors, int dim>
void KMeans<numvectors, dim>::print() const
{
    fAssignments.print(); 
    printClusterSizes();
}


template <int numvectors, int dim>
void KMeans<numvectors, dim>::printClusterSizes() const
{
    cout << "\tclusterSizes =";
    
    for (int k = 0; k < fK; ++k) 
        cout << ' ' << fClusters[k].size();

    cout << endl;
}





#endif /*KMEANS_H_*/
