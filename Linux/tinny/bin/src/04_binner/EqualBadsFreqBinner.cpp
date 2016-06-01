#include "EqualBadsFreqBinner.h"

#include <iostream>
#include <iomanip>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

EqualBadsFreqBinner::EqualBadsFreqBinner( int intVal ):
    numBads(0),
    numBins( intVal ),
    bad_pat( "\\s*[1YyFf]\\s*" )
{}

EqualBadsFreqBinner::~EqualBadsFreqBinner(){}

void EqualBadsFreqBinner::update( const string& strVal  ){

    vector<string> fieldsArray;
    split( fieldsArray, strVal, is_any_of("\t") );

    double doubleVal = atof( fieldsArray[0].c_str() );
    string tag = fieldsArray[1];

    if ( regex_match( tag, bad_pat ) ) {
        ++ numBads;
    }

    numMap.insert( pair<double,string>(doubleVal,tag) );

}

void EqualBadsFreqBinner::execute(){

    int binSize = static_cast<int>(static_cast<double>(numBads) / static_cast<double>(numBins) + 0.5);

    int badCount = 0;
    vector<double> cutOffs;

    cutOffs.push_back( (*numMap.begin()).first );

    double max = numeric_limits<double>::min();

    multimap<double,string>::const_iterator it;
    for ( it = numMap.begin(); it != numMap.end(); ++it ) {

        max = (*it).first > max? (*it).first : max;

        if ( regex_match( (*it).second, bad_pat )  ) {

            ++badCount;

            if( (badCount % binSize) == 0  ) {
                cutOffs.push_back( (*it).first );
            }

        }

    }

    //it = numMap.end();
    //--it;

    //if ( cutOffs.size() == numBins ) {

        for ( int i = 0; i < numBins-1; ++i ) {
            cout << "["   << cutOffs[i]
                 << " - " << cutOffs[i+1]
                 << "]"   << endl;
        }

        cout << "["   << cutOffs[numBins - 1]
             << " - " << max
             << "]"   << endl;

        //}
        //else {
        
        //for ( int i = 0; i < cutOffs.size()-2; ++i ) {
        //  cout << "["   << cutOffs[i]
        //       << " - " << cutOffs[i+1]
        //       << "]"   << endl;
        //}

        //cout << "["   << cutOffs[numBins - 1]
        //   << " - " << (*it).first
        //   << "]"   << endl;

        //}

}
