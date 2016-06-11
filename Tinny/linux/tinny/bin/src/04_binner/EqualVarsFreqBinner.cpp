#include "EqualVarsFreqBinner.h"

#include <iostream>
#include <iomanip>

using namespace std;

EqualVarsFreqBinner::EqualVarsFreqBinner( int intVal ):
    numBins( intVal )
{
    numArray.reserve(1000000);
}

EqualVarsFreqBinner::~EqualVarsFreqBinner(){}

void EqualVarsFreqBinner::update( const string& strVal  ){

    double doubleVal = atof( strVal.c_str() );
    
    numArray.push_back( doubleVal );

}

void EqualVarsFreqBinner::execute(){

    sort( numArray.begin(), numArray.end() );

    double step = 1.0 / numBins;

    int ix_from = 0;
    int ix_to   = 0;

    for ( int i = 0; i < numBins; ++i ) {

        if ( i == 0 ) {
            ix_from = 0;
            ix_to   = static_cast<int>( (i+1) * step * numArray.size() );
        }
        else if ( i == numBins - 1 ) {
            ix_from = static_cast<int>( i * step * numArray.size() );
            ix_to   = numArray.size() - 1;
        }
        else {
            ix_from = static_cast<int>( i * step * numArray.size() );
            ix_to   = static_cast<int>( (i+1) * step * numArray.size() );
        }

        cout << "["   << numArray[ ix_from ]
             << " - " << numArray[ ix_to ]
             << "]" << endl;

    }

}
