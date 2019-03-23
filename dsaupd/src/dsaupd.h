#ifndef DSAUPD_H_
#define DSAUPD_H_

/*
  In this header file, I have defined a simplified function
  call to the ARPACK solver routine for a simple, symmetric
  eigenvalue problem Av = lv.  The looping procedure and
  final extraction of eigenvalues and vectors is handled
  automatically.  Most of the parameters to the FORTRAN
  functions are hidden from the user, since most of them are
  determined from user input anyway.
  
  The remaining parameters to the function calls are as follows:
  
    dsaupd(int n, int nev, double *Evals)
    dsaupd(int n, int nev, doubel *Evals, double **Evecs)

    n: the order of the square matrix A
    nev: the number of eigenvalues to be found, starting at the
         bottom.  Note that the highest eigenvalues, or some
     other choices, can be found.  For now, the choice of
     the lowest nev eigenvalues is hard-coded.
    Evals: a one-dimensional array of length nev to hold the
           eigenvalues.
    Evecs: a two-dimensional array of size nev by n to hold the
           eigenvectors.  If this argument is not provided, the
       eigenvectors are not calculated.  Note that the
       vectors are stored as columns of Evecs, so that the
       elements of vector i are the values Evecs[j][i].

  The function is overdefined, so that you can call it in either
  fashion and the appropriate version will be run.

  To use these function calls, there must be a function
  defined in the calling program of the form

    av(int n, double *in, double *out)

  where the function av finds out = A.in, and n is the order of
  the matrix A.  This function must be defined before the
  statement that includes this header file, as it needs to know
  about this function.  It is used in the looping procedure.

  Scot Shaw
  30 August 1999
*/

#include <string>

using namespace std;


//----- reverse communication interface (aka callback)

class ArpackCallback
{
public:
    virtual void av(double *in, double *out) const = 0;
};



//----- C++ interface



template <class Eigen, class AC>
class Dsaupd
{
protected:

    int      fDim;
    int      fNumValues;
    
    Eigen&   fEigen;
    
public:

                 Dsaupd(Eigen& eigen); 
                 ~Dsaupd();
    
            void dsaupd(AC& ac);
            void print();
            void printValues();
            void printVectors();
            
            void saveValues(const string& filename) const;
            void saveVectors(const string& filename) const;

protected:

            void copyVectors(double *v);        
};



//----- ARPACK fortran routines

extern "C" void dsaupd_(int *ido, char *bmat, int *n, char *which,
            int *nev, double *tol, double *resid, int *ncv,
            double *v, int *ldv, int *iparam, int *ipntr,
            double *workd, double *workl, int *lworkl,
            int *info);

extern "C" void dseupd_(int *rvec, char *All, int *select, double *d,
            double *v, int *ldv, double *sigma, 
            char *bmat, int *n, char *which, int *nev,
            double *tol, double *resid, int *ncv, double *v,
            int *ldv, int *iparam, int *ipntr, double *workd,
            double *workl, int *lworkl, int *ierr);


template <class Eigen, class AC>
Dsaupd<Eigen, AC>::Dsaupd(Eigen& eigen)
    : fEigen(eigen),
      fDim(eigen.n()),
      fNumValues(eigen.numValues())
{}

template <class Eigen, class AC>
Dsaupd<Eigen, AC>::~Dsaupd()
{}


template <class Eigen, class AC>
void Dsaupd<Eigen, AC>::dsaupd(AC& ac)
{
    int n   = fDim;
    int nev = fNumValues;
      
    int ido      = 0;     /* Initialization of the reverse communication parameter. */
  
    char bmat[2] = "I";   /* Specifies that the right hand side matrix
                             should be the identity matrix; this makes
                             the problem a standard eigenvalue problem.
                             Setting bmat = "G" would have us solve the
                             problem Av = lBv (this would involve using
                             some other programs from BLAS, however). */
    

    char which[3] = "SM"; /* Ask for the nev eigenvalues of smallest magnitude.  The possible options are
                               LM: largest magnitude
                               SM: smallest magnitude
                               LA: largest real component
                               SA: smallest real compoent
                               LI: largest imaginary component
                               SI: smallest imaginary component */

    double tol = 0.0;     /* Sets the tolerance; tol<=0 specifies machine precision */

    double* resid = new double[n];

    int    ncv = 4*nev;   /* The largest number of basis vectors that will
                              be used in the Implicitly Restarted Arnoldi
                              Process.  Work per major iteration is
                              proportional to N*NCV*NCV. */
    if (ncv>n) ncv = n;
    
    int     ldv = n;
    
    double* v = new double[ldv*ncv];
    
    int     iparam[11];  /* An array used to pass information to the routines
                            about their functional modes. */
    iparam[0] = 1;      // Specifies the shift strategy (1->exact)
    iparam[2] = 3*n;    // Maximum number of iterations
    iparam[6] = 1;      /* Sets the mode of dsaupd.
                              1 is exact shifting,
                              2 is user-supplied shifts,
                              3 is shift-invert mode,
                              4 is buckling mode,
                              5 is Cayley mode. */

    int     ipntr[11];  /* Indicates the locations in the work array workd
                              where the input and output vectors in the
                              callback routine are located. */

    double *workd = new double[3*n];

    int     lworkl = ncv*(ncv+8);   /* Length of the workl array */
    double *workl = new double[lworkl];
    
    int     info = 0;   /* Passes convergence information out of the iteration routine. */
    int     rvec = fEigen.hasVectors();   /* Specifies that eigenvectors should/should not be calculated */
    
    int    *select = new int[ncv];
    double *d = new double[2*ncv];   /* This vector will return the eigenvalues from
                            the second routine, dseupd. */
    
    double  sigma;
    int     ierr;
   
    /* Here we enter the main loop where the calculations are
       performed.  The communication parameter ido tells us when
       the desired tolerance is reached, and at that point we exit
       and extract the solutions. */

    do {
        dsaupd_(&ido, bmat, &n, which, &nev, &tol, resid, 
            &ncv, v, &ldv, iparam, ipntr, workd, workl,
            &lworkl, &info);
        
        if ((ido==1)||(ido==-1)) ac.av(workd+ipntr[0]-1, workd+ipntr[1]-1);
    } while ((ido==1)||(ido==-1));
    
    if (info<0) 
    {
        cout << "Error with dsaupd, info = " << info << "\n";
        cout << "Check documentation in dsaupd\n\n";
    }
    else 
    {
        dseupd_(&rvec, "All", select, d, v, &ldv, &sigma, bmat,
            &n, which, &nev, &tol, resid, &ncv, v, &ldv,
            iparam, ipntr, workd, workl, &lworkl, &ierr);
    
        if (ierr!=0) 
        {
            cout << "Error with dseupd, info = " << ierr << "\n";
            cout << "Check the documentation of dseupd.\n\n";
        } 
        else if (info==1) 
        {
            cout << "Maximum number of iterations reached.\n\n";
        } 
        else if (info==3) 
        {
            cout << "No shifts could be applied during implicit\n";
            cout << "Arnoldi update, try increasing NCV.\n\n";
        }
    
        // copy the eigenvalues/vectors to member vars
        
        for (int i = 0; i < fNumValues; ++i) 
        {
            fEigen.setValue(i, d[i]);
            
            for (int j = 0; j < fDim; ++j) 
                fEigen.setVectorComponent(i, j, v[i*fDim+j]);
        }
    }
    
    delete d;
    delete select;
    delete workl;
    delete workd;
    delete v;
    delete resid;
}


#endif /*DSAUPD_H_*/
