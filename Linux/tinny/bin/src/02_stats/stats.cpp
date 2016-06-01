#include <iostream>
#include <iomanip>

#include <string>
#include <cmath>

#include "median_torben.hpp"

using namespace std;

void printUsage();

int main(int argc, char **argv) {

    if ( argc > 1 ) {

        printUsage();
        return 0;

    }

    // calculating ...
    string line;
    vector<double> elemList;

    elemList.reserve(1000000);

    // first line for efficiency
    long tot = 1;
    getline( cin, line );

    double val  = atof( line.c_str() );
    double min  = val;
    double max  = val;
    double mean = val;
    double m2   = 0;

    // the rest lines
    while ( getline( cin, line ) ) {

        ++tot;

        val = atof( line.c_str() );
        
        elemList.push_back( val );

        min = val < min ? val : min;
        max = val > max ? val : max;

        double delta = val - mean;
        mean = mean + delta / tot;
        m2 = m2 + delta * ( val - mean );

    }

    double variance = m2 / (tot-1);
    double stdd = sqrt( variance );
    double median = torben( elemList );

    // print out
    //cout << setiosflags(ios::fixed) << setprecision(4);
    cout << "Min:\t"    << fixed << setprecision(4) << min << endl;
    cout << "Max:\t"    << fixed << setprecision(4) << max << endl;
    cout << "Mean:\t"   << fixed << setprecision(4) << mean << endl;
    cout << "Median:\t" << fixed << setprecision(4) << median << endl; 
    cout << "StdD:\t"   << fixed << setprecision(4) << stdd << endl;
    cout << "Var:\t"    << fixed << setprecision(4) << variance << endl;

}

void printUsage() {

    cout << "Usage:\t" << "cat dataset | cut -f n | stats " << endl;

}
