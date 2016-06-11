#include<iostream>
#include <unistd.h>

#include "Binner.h"
#include "EqualWidthBinner.h"
#include "EqualVarsFreqBinner.h"
#include "EqualBadsFreqBinner.h"
//#include "EntropyBinner.h"
//#include "Chi2Binner.h"
//#include "KMeansCluster.h"
//#include "FuzzyCMeansCluster.h"

using namespace std;

void printUsage();

auto_ptr<Binner> binnerFactory( int , int, int );

int main( int argc, char **argv ) {

    int nVal = -1;
    int cVal = -1;
    int numBins = 10;

    int intVal;
    
    // command line parsing
    while ((intVal = getopt(argc, argv, "hn:c:b:")) != -1) {

        switch (intVal) {

        case 'n':
            nVal = atoi( optarg );
            break;
        case 'c':
            cVal = atoi( optarg );
            break;
        case 'b':
            numBins = atoi( optarg );
            break;
        case 'h':
            printUsage();
            return 0;
        default:
            printUsage();
            return 1;
        }

    }

    // get a binner
    auto_ptr<Binner> binner = binnerFactory( nVal, cVal, numBins );

    if ( NULL == binner.get()) {
        return 1;
    }

    // updating ...
    string line;
    while ( getline( cin, line ) ) {
        binner->update( line );
    }
    
    // displaying ...
    binner->execute();

}

void printUsage() {

    cout << "Usage:\tcat dataset | cut -f n1 [,n2] | binner" << endl;
    cout << "\t-h\tHelp" << endl;
    cout << "\t-b\tNumber of Bins" << endl;
    cout << "\t-n\t0\tEqual Width Binning" << endl;
    cout << "\t\t1\tEqual Vars Frequency Binning" << endl;
    cout << "\t\t2\tEqual Bads Frequency Binning" << endl;
    cout << "\t\t3\tEntropy Binning" << endl;
    cout << "\t\t4\tChi2 Binning" << endl;
    cout << "\t-c\t0\tK-Means Clustering" << endl;
    cout << "\t\t1\tFuzzy-C-Means Clustering" << endl;

}


auto_ptr<Binner> binnerFactory( int n, int c, int numBins) {

    auto_ptr<Binner> binner( NULL );

    if ( n < 0 and c < 0 ) {

        printUsage();
        return binner;

    }
    else if ( n >= 0 and c >= 0 ) {

        cerr << "Variable value must be numeric or categorical." << endl;
        printUsage();
        return binner;

    }
    else if ( n == 0 ) {

        binner.reset( new EqualWidthBinner( numBins ) );

    }
    else if ( n == 1 ) {

        binner.reset( new EqualVarsFreqBinner( numBins ) );

    }
    else if ( n ==2 ) {

        binner.reset( new EqualBadsFreqBinner( numBins ) );

    }
    else if ( n == 3 ) {

        cerr << "Entropy binning ... (not yet implemented)" << endl;
        return binner;

    }
    else if ( n ==4 ) {

        cerr << "Chi2 binning ... (not yet implemented)" << endl;

    }
    else if ( c == 0 ) {
        
        cerr << "K-means clustering ... (not yet implemented)" << endl;
        return binner;

    }
    else if ( c == 1 ) {

        cerr << "Fuzzy-C-means clustering ... (not yet implemented)" << endl;
        return binner;

    }
    else {

        printUsage();
        return binner;

    }

    return binner;

}
