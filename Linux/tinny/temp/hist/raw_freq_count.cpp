#include<iostream>
#include<string>
#include<map>
#include<boost/regex.hpp>

#define SPACE "     "

using namespace std;
using namespace boost;

int main() {
    
    string line;
    map<string,int> nameCount;

    nameCount[ SPACE ] = 0;
    regex e1("^\\s+$");
    regex e2("^$");
    int total_num = 0;

    while ( cin ) {

	getline( cin, line );
	++total_num;

	//while ( line.find(" ") != string::npos ) {
	//  line.replace(line.find(" "),1,"");
	//}

	if ( regex_match(line,e1) ||
             regex_match(line,e2) ) {
	    ++nameCount[ SPACE ];
	}
	//else if ( line.empty() ) {
	//    ++nameCount[ SPACE ];
	//}
	else if ( nameCount.find( line ) == nameCount.end() ) {
	    nameCount[ line ] = 1;
	}
	else {
	    ++nameCount[ line ];
	}

    }

    // remove the last newline count
    --nameCount[ SPACE ];
    --total_num;


    //constructing a reversed map
    multimap<int,string> output;

    map<string,int>::const_iterator it;
    for ( it = nameCount.begin(); it != nameCount.end(); ++it ) {
	output.insert( pair<int,string>( (*it).second,(*it).first ) ) ;
    }

    multimap<int,string>::reverse_iterator rit;
    for ( rit = output.rbegin(); rit != output.rend(); ++rit ) {
	if ( (*rit).first > 0  ) {
	    cout << (*rit).second << "\t" 
		 << (*rit).first  << "\t"
		 << (*rit).first * 100.0 / total_num
		 <<  endl;
	}
    }

    //cout << "total number of lines:\t" << total_num << endl;
    //cout << "max size of map is:\t" << nameCount.max_size() << endl;
    //cout << "max size of multi-map is:\t" << output.max_size() << endl;

}
