#include<iostream>
#include <iomanip>
#include<string>
#include<map>
#include<boost/regex.hpp>

#include "CatHistInfo.h"
//#include "Utils.h"

using namespace std;
using namespace boost;

CatHistInfo::CatHistInfo(): totNum(0), blank_pat( "^\\s*$" ) {}

CatHistInfo::~CatHistInfo(){}

void CatHistInfo::update( const std::string& strVal ){

    ++totNum;

    if ( regex_match(strVal,blank_pat ) ) {

        if ( freqCount.find( "#BLANK#" ) == freqCount.end() ) {
            freqCount[ "#BLANK#" ] = 1;
        }
        else {
            ++freqCount[ "#BLANK#" ]; 
        }

    }
    else if ( freqCount.find( strVal ) == freqCount.end() ) {
        freqCount[ strVal ] = 1;
    }
    else {
        ++freqCount[ strVal ];
    }

}

void CatHistInfo::display() const{

    // constructing a reversed map
    multimap<long,string> output;

    map<string,long>::const_iterator it;
    for ( it = freqCount.begin(); it != freqCount.end(); ++it ) {
	output.insert( pair<long,string>( (*it).second,(*it).first ) ) ;
    }

    // print the headline
    cout << "symbol" << "\t"
	 << "#rec" << "\t"
	 << "%rec" << "\t"
	 << "%cum" << endl;

    // print it out
    double perc = 0;
    double cum_perc = 0;

    multimap<long,string>::reverse_iterator rit;
    for ( rit = output.rbegin(); rit != output.rend(); ++rit ) {

	perc = (*rit).first * 100.0 / totNum;
	cum_perc += perc;

	if ( (*rit).first > 0  ) {
	    cout << (*rit).second << "\t" 
		 << (*rit).first  << "\t"
		 << setiosflags(ios::fixed) << setprecision(2)
		 << perc << "\t"
		 << cum_perc << endl;
	}

    }


}
