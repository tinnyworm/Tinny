#include<iostream>
#include <iomanip>
#include<string>
#include<map>
#include<boost/regex.hpp>

#include "NumHistInfo.h"

using namespace std;
using namespace boost;

NumHistInfo::NumHistInfo( int size ):
    totNum(0),
    numBlank(0),
    numInvalid(0),
    binSize(size),
    blank_pat( "^\\s*$" ),
    number_pat( "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?" )
{}

NumHistInfo::~NumHistInfo(){}

void NumHistInfo::update( const std::string& line ){

    ++totNum;

    string strVal = line;

    while ( strVal.find(" ") != string::npos ) {            // remove spaces
        strVal.replace(strVal.find(" "),1,"");
    }

    if ( regex_match(strVal,blank_pat) ) {                  // blank lines
        ++numBlank;
    }
    else if ( ! regex_match(strVal,number_pat) ){           // invalid number
        ++numInvalid;
    }
    else {

        double val = atof( strVal.c_str() );
        int binned_val = binSize * static_cast<int>( val / binSize );

        if ( freqCount.find( binned_val ) == freqCount.end() ) {
            freqCount[ binned_val ] = 1;
        }
        else {
            ++freqCount[ binned_val ];
        }

    }

    
}

void NumHistInfo::display() const{

    // print the headline
    cout << "symbol" << "\t"
	 << "#rec" << "\t"
	 << "%rec" << "\t"
	 << "%cum" << endl;

    // print it out
    double perc = 0;
    double cum_perc = 0;

    perc = numBlank * 100.0 / totNum;
    cum_perc += perc;

    if ( numBlank > 0 ) {
	cout << "#BLANK#\t" 
	     << numBlank << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << perc << "\t"
	     << cum_perc << endl;
    }

    perc = numInvalid * 100.0 / totNum;
    cum_perc += perc;

    if ( numInvalid > 0 ) {
        cout << "#INVAL#\t"
             << numInvalid << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
             << perc << "\t"
	     << cum_perc << endl;
    }

    map<int,long>::const_iterator it;
    for ( it = freqCount.begin(); it != freqCount.end(); ++it ) {

	perc = (*it).second * 100.0 / totNum;
	cum_perc += perc;

	cout << (*it).first << "\t"
	     << (*it).second << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << perc << "\t"
	     << cum_perc << endl;
	
    }

}
