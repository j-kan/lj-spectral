#include <iostream>
#include <fstream>
#include <set>

using namespace std;

const int n = 297987;

typedef set<int,greater<int> > IntSet;

IntSet* edges;


void addEdge(int i, int j)
{
    if (j < i)
    {
        int tmp = i;
        i = j;
        j = tmp;
    }
    edges[i].insert(j);
}


void read()
{
    while (1)
    {
        int i = -1, j = -1;
        
        if (!(cin >> i)) break;
        if (!(cin >> j)) break;
        if (i == -1)     break;
        if (j == -1)     break;
            
        addEdge(i-1, j-1);
    }  
}

void write()
{
    for (int i = 0; i < n; ++i)
    {
        IntSet::const_iterator it;
        
        for (it = edges[i].begin(); it != edges[i].end(); ++it)
        {
            cout << i+1 << '\t' << *it+1 << '\n';
        }
    }
}

int main(int argc, char **argv) 
{
    edges = new IntSet[n];
    
    read();
    write();
    
    delete edges;
}

