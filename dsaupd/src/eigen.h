#ifndef EIGEN_H_
#define EIGEN_H_


//template <int dim, int num_values>
//class Eigenvalues
//{
//    double fValues[num_values];
//
//public:
//
//    public int  numValues() const                 { return num_values;  }
//    public int  n() const                         { return dim;         }
//    public bool hasVectors() const                { return false;       } 
//    
//    public void   setValue(int i, double value)   { fValues[i] = value; }   
//    public double getValue(int i) const           { return fValues[i];  }   
//
//    public void   setVectorComponent(int i, int j, double value)   {}   
//};


template <int dim, int num_values>  // dim = number of rows/users
class EigenPairs
{
public:

    typedef double   ValueRow[num_values];
    typedef ValueRow EigenValues;
    typedef ValueRow EigenVectors[dim];

private: 
    
    EigenValues  fValues;
    EigenVectors fVectors;

public:

    EigenPairs()
    {
        for (int j = 0; j < num_values; ++j)
        {
            fValues[j] = 0;
            
            for (int i = 0; i < dim; ++i)
            { 
                fVectors[i][j] = 0;
            }
        }
    }
    
    int  numValues() const  { return num_values; }
    int  n() const          { return dim;        }
    bool hasVectors() const { return true;       }    
    
    void   setValue(int i, double value)   { fValues[i] = value; }   
    double getValue(int i) const           { return fValues[i];  }   

    void   setVectorComponent(int i, int j, double value)   { fVectors[j][i] = value; }   
    
    void   print() const                   { printValues(); printVectors(); }
    void   printValues() const;
    void   printVectors() const;

    void   saveValues(const string& filename) const;
    void   saveVectors(const string& filename) const;
    void   readValues(const string& filename);
    void   readVectors(const string& filename);
    
    const EigenVectors& vectors() const { return fVectors; }
    EigenVectors& vectors()             { return fVectors; }
};


template <int dim, int num_values>
void EigenPairs<dim, num_values>::printValues() const
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
void EigenPairs<dim, num_values>::printVectors() const
{
    cout << "\neigenvectors = cbind(\n";
    
    for (int i = 0; i < num_values; ++i) 
    {
        cout << "\tc(";

        for (int j = 0; j < dim; ++j)
        {
            if (j > 0)
                cout << ", ";
                
            cout << fVectors[j][i];
        }
        if (i < num_values-1)
            cout << "),\n";
        else
            cout << ")\n";
    }
    cout << ")\n";
}


template <int dim, int num_values>
void EigenPairs<dim, num_values>::saveValues(const string& filename) const
{
    ofstream ofs(filename.c_str());
  
    if (!ofs)
        throw "failed to open file for writing";

    //ofs.setf(ios::fixed, ios::floatfield);
    ofs.precision(20);
    
    for (int i = 0; i < num_values; ++i)
    { 
        if (i > 0)
            ofs << "\t";
        
        ofs << fValues[i];
    }
    ofs << "\n";
}



template <int dim, int num_values>
void EigenPairs<dim, num_values>::saveVectors(const string& filename) const
{
    ofstream ofs(filename.c_str());
  
    if (!ofs)
        throw "failed to open file for writing";

    //ofs.setf(ios::fixed, ios::floatfield);
    ofs.precision(20);
    
    for (int j = 0; j < dim; ++j)
    {
        for (int i = 0; i < num_values; ++i) 
        {
            ofs << fVectors[j][i] << ' ';
        }
        ofs << '\n';
    }
}


template <int dim, int num_values>
void EigenPairs<dim, num_values>::readValues(const string& filename) 
{
    ifstream ifs(filename.c_str());
  
    if (!ifs)
        throw "failed to open eigenvalues file";
      
    int i = 0;
    
    while (ifs >> fValues[i++])
    {
        if (i > num_values)
            break;
    }
}



template <int dim, int num_values>
void EigenPairs<dim, num_values>::readVectors(const string& filename) 
{
    ifstream ifs(filename.c_str());
  
    if (!ifs)
        throw "failed to open eigenvectors file";

    for (int j = 0; j < dim; ++j)
    {
        for (int i = 0; i < num_values; ++i) 
        {
            ifs >> fVectors[j][i];
        }
    }
}


#endif /*EIGEN_H_*/
