#include<iostream>
#include <iomanip>

#include<string>
#include<map>
#include<boost/regex.hpp>

using namespace std;
using namespace boost;

void printUsage();

int main(int argc, char **argv) {

    if ( argc > 1 ) {

        printUsage();
        return 1;

    }

    map<string,long> freqCount;
    freqCount[ "Blank" ]   = 0;
    freqCount[ "Numeric" ] = 0;
    freqCount[ "Nominal" ] = 0;

    regex blank_pat( "^\\s*$" );
    regex number_pat( "[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?" );
    //regex number_pat( "" );

    string line;
    long total_num = 0;

    while ( getline( cin, line ) ) {

        ++total_num;

        if ( regex_match(line,blank_pat ) ) {
	    ++freqCount[ "Blank" ];
	}
	else if ( regex_match(line,number_pat ) ) {
	    ++freqCount[ "Numeric" ];
	}
	else {
	    ++freqCount[ "Nominal" ];
	}

    }

    // print the headline
    cout << "name" << "\t"
	 << "#rec" << "\t"
	 << "%rec" << "\t"
	 << "%cum" << endl;
    
    // print it out
    double perc = 0;
    double cum_perc = 0;

    map<string,long>::iterator it;
    for ( it = freqCount.begin(); it != freqCount.end(); ++it) {

	perc = (*it).second * 100.0 / total_num;
	cum_perc += perc;

	if ( (*it).second > 0  ) {
	    cout << (*it).first  << "\t" 
		 << (*it).second << "\t"
		 << setiosflags(ios::fixed) << setprecision(2)
		 << perc << "\t"
		 << cum_perc << endl;
	}
        
    }

}

void printUsage() {

    cout << "Usage:\t" << "cat dataset | cut -f n | check_dqr " << endl;

}
