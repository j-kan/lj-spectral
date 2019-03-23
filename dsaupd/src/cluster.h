#ifndef CLUSTER_H_
#define CLUSTER_H_

static void cluster(const GraphBase& g, EigenPairs<n, nev>::EigenVectors& vectors, const string& fOutputDir, int fKmeansNumRestarts, bool fKmeansNormalize = false, int fKmeansMaxIterations=1000)
{
    //boost::filesystem::create_directory(dir.c_str());

    for (int k = 32; k <= 256; k *= 2)  // number of clusters
    {
        double q[fKmeansNumRestarts];
        double maxQ = -1;
        Assign bestAssignments;
    
        for (int i = 0; i < fKmeansNumRestarts; ++i)
        { 
            cout << "\n\n---------------------- K-means[" << k << "] " << i << "\n\n" << flush;
            
            Clusterer  *kmeans = new Clusterer(vectors, k, fKmeansNormalize);
            
            kmeans->run(fKmeansMaxIterations);
            kmeans->print();
    
            q[i] = g.modularity(k, kmeans->assignmentArray());
            
            cout << "=====> modularity: " << q[i] << endl;
            
            if (q[i] > maxQ)
            {
                maxQ = q[i];
                bestAssignments = kmeans->assignments(); 
            }
            
            delete kmeans;                
        }
    
        cout << "\n\n---------------------- K-means results:\n";
    
        for (int i = 0; i < fKmeansNumRestarts; ++i)
        {
            cout << "\t" <<  q[i];

            if (q[i] == maxQ)
                cout << " <==";
                
            cout << endl;
        }
        
        stringstream path;
        
        path << fOutputDir << "/assignments-k-" << k << "-Q-" << maxQ << ".txt";
        
        bestAssignments.save(path.str());
    }
}

#endif /*CLUSTER_H_*/
