#ifndef ASSIGNMENTS_H_
#define ASSIGNMENTS_H_

#include <string>
#include <fstream>

using namespace std;


template <int numvectors>
class AssignmentVec
{
private:

    int     fAssignments[numvectors];

public:

    AssignmentVec()  
    { 
        for (int i = 0; i < numvectors; ++i) 
            fAssignments[i] = 0; 
    }
        
    AssignmentVec(const AssignmentVec& a)
    { 
        for (int i = 0; i < numvectors; ++i) 
            fAssignments[i] = a[i]; 
    }

    ~AssignmentVec() {}
    
    const AssignmentVec& operator=(const AssignmentVec& a)
    {
        for (int i = 0; i < numvectors; ++i) 
            fAssignments[i] = a[i]; 
        
        return *this;
    }
    
    int     operator[](int i) const { return fAssignments[i]; }
    int&    operator[](int i)       { return fAssignments[i]; }
    
    const int *   asArray() const   { return fAssignments;    }
    
    void read(const string& filename)
    {
        ifstream ifs(filename.c_str());
      
        if (!ifs)
            throw "failed to open assignment file";
          
        int i = 0;
        int a;
        
        while (ifs >> fAssignments[i++])
        {
            if (i > numvectors)
                break;
        }
    }  
    
    void print() const
    {
        cout << "\tassignments =";
    
        for (int i = 0; i < numvectors; ++i)
        {
//            if (i > 0) 
//                cout << ", ";
                 
            cout << ' ' << fAssignments[i];
        }
        
        cout << '\n';    
    }
    
    void save(const string& filename) const
    {
        ofstream ofs(filename.c_str());
      
        if (!ofs)
            throw "failed to open file for writing";
    
        for (int i = 0; i < numvectors; ++i)
            ofs << fAssignments[i] << '\n';
    }
    
};



#endif /*ASSIGNMENTS_H_*/
