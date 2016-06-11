#include<iostream>
#include <iomanip>
#include<string>
#include<map>
#include<boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "CatTaggedHistInfo.h"

using namespace std;
using namespace boost;

CatTaggedHistInfo::CatTaggedHistInfo(): 
    totNum(0),
    blank_pat( "^\\s*$" ),
    bad_pat( "\\s*[1YyFf]\\s*" )
{}

CatTaggedHistInfo::~CatTaggedHistInfo(){}

void CatTaggedHistInfo::update( const std::string& strVal ){

    ++totNum;

    vector<string> fieldsArray;   
    split( fieldsArray, strVal, is_any_of("\t") );

    string name = fieldsArray[0];
    string tag  = fieldsArray[1];

    if ( regex_match( name, blank_pat ) ) {

        if ( allCount.find( "#BLANK#" ) == allCount.end() ) {

            allCount[ "#BLANK#" ] = 1;

            if ( regex_match(tag,bad_pat) ) {
                badCount[ "#BLANK#" ] = 1;
            }
            else {
                badCount[ "#BLANK#" ] = 0;
            }

        }
        else {

            ++allCount[ "#BLANK#" ];

            if ( regex_match(tag,bad_pat) ) {
                ++badCount[ "#BLANK#" ];
            }

        }

    }
    else {

        if ( allCount.find( name ) == allCount.end() ) {

            allCount[ name ] = 1;

            if ( regex_match(tag,bad_pat) ) {
                badCount[ name ] = 1;
            }
            else {
                badCount[ name ] = 0;
            }

        }
        else {

            ++allCount[ name ];

            if ( regex_match(tag,bad_pat) ) {
                ++badCount[ name ];
            }

        }

    }

} // end of update

void CatTaggedHistInfo::display() const {

    //constructing a reversed map
    multimap<long,string> output;
    
    map<string,long>::const_iterator it;
    for ( it = allCount.begin(); it != allCount.end(); ++it ) {
	output.insert( pair<long,string>( (*it).second,(*it).first ) ) ;
    }

    // print the headline
    cout << "symbol" << "\t"
	 << "#bads"  << "\t"
	 << "#rec"   << "\t"
	 << "%bads"  << "\t"
	 << "%rec"   << "\t"
	 << "%cum"   << endl;

    // print it out
    double perc_bads = 0;
    double perc_all  = 0;
    double cum_perc  = 0;

    multimap<long,string>::reverse_iterator rit;
    for ( rit = output.rbegin(); rit != output.rend(); ++rit ) {

        perc_bads = badCount.find( (*rit).second )->second * 100.0 / (*rit).first;
        perc_all  = (*rit).first * 100.0 / totNum;
        cum_perc  += perc_all;

        if ( (*rit).first > 0  ) {
            cout << (*rit).second << "\t"
                 << badCount.find( (*rit).second )->second << "\t"
                 << (*rit).first  << "\t"
                 << setiosflags(ios::fixed) << setprecision(2)
                 << perc_bads << "\t"
                 << perc_all << "\t"
                 << cum_perc <<  endl;
        }

    }

} // end of display
