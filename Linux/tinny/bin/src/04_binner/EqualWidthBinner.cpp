#include "EqualWidthBinner.h"

#include<iostream>
#include <iomanip>

using namespace std;

EqualWidthBinner::EqualWidthBinner( int intVal ):
    numBins( intVal ),
    min( numeric_limits<double>::max() ),
    max( numeric_limits<double>::min() ) {}

EqualWidthBinner::~EqualWidthBinner(){}

void EqualWidthBinner::update( const string& strVal  ){

    double doubleVal = atof( strVal.c_str() );
    
    min = doubleVal < min ? doubleVal : min;
    max = doubleVal > max ? doubleVal : max;

}

void EqualWidthBinner::execute(){

    double step = ( max - min ) / numBins;

    for ( int i = 0; i < numBins; ++i ) {

        cout << "["   << min + step * i 
             << " - " << min + step * ( i + 1)
             << "]" << endl; 

    }
    
}
