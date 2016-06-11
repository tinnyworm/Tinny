#include<iostream>
#include <iomanip>
#include<string>
#include<map>
#include<boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "NumTaggedHistInfo.h"

using namespace std;
using namespace boost;

NumTaggedHistInfo::NumTaggedHistInfo( int size ): 
    totNum(0),
    numBlank(0),
    numBadBlank(0),
    numInvalid(0),
    numBadInvalid(0),
    binSize(size),
    blank_pat( "^\\s*$" ),
    number_pat( "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?" ),
    bad_pat( "\\s*[1YyFf]\\s*" )
{}

NumTaggedHistInfo::~NumTaggedHistInfo(){}

void NumTaggedHistInfo::update( const std::string& strVal ){

    ++totNum;

    //string line = strVal;

    vector<string> fieldsArray; 
    split( fieldsArray, strVal, is_any_of("\t") );

    string name = fieldsArray[0];
    string tag  = fieldsArray[1];

    while ( name.find(" ") != string::npos ) {              // remove spaces
        name.replace(name.find(" "),1,"");
    }

    if ( regex_match(name,blank_pat) ) {                    // blank lines
        ++numBlank;
        if ( regex_match(tag,bad_pat) ) {
            ++numBadBlank;
        }
    }
    else if ( ! regex_match(name,number_pat) ){             // invalid number
        ++numInvalid;
        if ( regex_match(tag,bad_pat) ) {
            ++numBadInvalid;
        }
    }
    else {

        double val = atof( name.c_str() );
        int binned_val = binSize * static_cast<int>( val / binSize );
	    
        if ( allCount.find( binned_val ) == allCount.end() ) {
            
            allCount[ binned_val ] = 1;
        
            if ( regex_match(tag,bad_pat) ) {
                badCount[ binned_val ] = 1;
            }
            else {
                badCount[ binned_val ] = 0;
            }
            
        }
        else {

            ++allCount[ binned_val ];

            if ( regex_match(tag,bad_pat) ) {
                ++badCount[ binned_val ];
            }
            
        }
	    
    } // end of else

} // end of update

void NumTaggedHistInfo::display() const {

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

    perc_bads = numBadBlank * 100.0 / numBlank;
    perc_all  = numBlank * 100.0 / totNum;
    cum_perc += perc_all;

    if ( numBlank > 0 ) {
	cout << "#BLANK#\t"
	     << numBadBlank << "\t"
	     << numBlank << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << perc_bads << "\t"
	     << perc_all << "\t"
	     << cum_perc << endl;
    }

    perc_bads = numBadInvalid * 100.0 / numInvalid;
    perc_all  = numInvalid * 100.0 / totNum;
    cum_perc += perc_all;

    if ( numInvalid > 0 ) {
        cout << "#INVAL#\t"
	     << numBadInvalid << "\t"
             << numInvalid << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << perc_bads << "\t"
             << perc_all << "\t"
	     << cum_perc << endl;
    }

    map<int,long>::const_iterator it;
    for ( it = allCount.begin(); it != allCount.end(); ++it ) {

	perc_bads = badCount.find( (*it).first )->second * 100.0 / (*it).second;
	perc_all  = (*it).second * 100.0 / totNum;
	cum_perc += perc_all;

	cout << (*it).first << "\t"
	     << badCount.find( (*it).first )->second << "\t"
	     << (*it).second << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << perc_bads << "\t"
	     << perc_all << "\t"
	     << cum_perc << endl;

    }


} // end of display
