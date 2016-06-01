#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

int main () {

    string line;
    vector<string> fieldsArray;   

    regex e("\\d");
    
    //ifstream myfile("example.txt");

    //if ( myfile.is_open() ){
	
    //while (! myfile.eof() ){
    //    getline (myfile,line);
    //    cout << line << endl;
    //}

    //myfile.close();

    //}
    //else {
    //cout << "Unable to open file"; 
    //}

    while ( cin ) {

	getline( cin, line );
	split( fieldsArray, line, is_any_of("\t") );

	vector<string>::const_iterator it;

	for( it = fieldsArray.begin(); it != fieldsArray.end(); ++it ) {
	    cout << *it << ",";
	}

	cout << endl;

    }

    return 0;

}
