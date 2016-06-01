#include<iostream>
#include <iomanip>
#include<cstdlib>
#include<string>
#include<map>
#include<boost/regex.hpp>
#include "anyoption.h"

using namespace std;
using namespace boost;

int main( int argc, char* argv[] ) {

    /////////////////////////////////////////////////////////
    // check options
    /////////////////////////////////////////////////////////

    AnyOption *opt = new AnyOption();

    opt->addUsage( "" );
    opt->addUsage( "Usage: " );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help\t\tPrints this help " );
    opt->addUsage( " -b  --bin bin_size\tBin Size " );
    opt->addUsage( "" );
    
    opt->setFlag( "help", 'h' );
    opt->setOption( "bin", 'b' );

    opt->processCommandArgs( argc, argv );

    // print usage if no options
    if( ! opt->hasOptions()) {
	opt->printUsage();
	delete opt;
	return 0;
    }
    if( opt->getFlag( "help" ) || opt->getFlag( 'h' ) ) {
	opt->printUsage();
	delete opt;
	return 0;
    }

    int bin_size;

    if( opt->getValue( "bin" ) != NULL || opt->getValue( 'b' ) != NULL ) {
	bin_size = atoi( opt->getValue( "bin" ) );
    }
    else {
	bin_size = 10;
    }

    delete opt;

    //////////////////////////////////////////////////////////
    // bin count
    //////////////////////////////////////////////////////////

    string line;
    vector<double> inputVals;
    inputVals.reserve( 100000 );

    int total_num = 0;
    int num_blank_lines = 0;
    int num_invalid_lines = 0;

    regex e1("^$");						      // empty line
    regex e2("^(\\d+\\.?\\d*|\\.\\d+)$");			      // valid number

    while ( getline( cin, line ) ) {

	++total_num;

	// remove spaces
	while ( line.find(" ") != string::npos ) {
	    line.replace(line.find(" "),1,"");
	}

	if ( regex_match(line,e1) ) {				      // blank lines
	    ++num_blank_lines;
	}
	else if ( ! regex_match(line,e2) ){			      // invalid number
	    ++num_invalid_lines;
	}
	else {

	    double val = atof( line.c_str() );
	    inputVals.push_back(val);

	}
	    
    }
    
    sort ( inputVals.begin(), inputVals.end() ); 

    double equal_width_step = ( inputVals.back() - inputVals.front() ) / bin_size;
    double equal_depth_step = 1.0 / bin_size;

    if ( num_blank_lines > 0 ) {
	cout << "Blank\t" 
	     << num_blank_lines << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
	     << num_blank_lines * 100.0 / total_num
	     << endl;
    }

    if ( num_invalid_lines > 0 ) {
        cout << "Invalid\t"
             << num_invalid_lines << "\t"
	     << setiosflags(ios::fixed) << setprecision(2)
             << num_invalid_lines * 100.0 / total_num
             << endl;
    }

    cout << "Equal Depth Cut-Offs:" << endl;

    for ( int i = 0; i < (bin_size -1); ++i ) {
	int idx = static_cast<int>(equal_depth_step * (i+1) * inputVals.size());
	cout << inputVals[idx] << "\t";
    }
    cout << endl;

    cout << "Equal Width Cut-Offs:" << endl;

    for ( int i = 0; i < (bin_size -1); ++i ) {
	cout << setiosflags(ios::fixed) << setprecision(2)
	     << inputVals.front() + equal_width_step * (i+1) << "\t";
    }
    cout << endl;

    return 0;

}
