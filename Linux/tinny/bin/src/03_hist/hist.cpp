#include <iostream>
#include <unistd.h>

#include "CatHistInfo.h"
#include "CatTaggedHistInfo.h"
#include "NumHistInfo.h"
#include "NumTaggedHistInfo.h"

using namespace std;

void printUsage();

int main(int argc, char **argv) {

    int tFlag = 0;
    int nFlag = 0;
    int binSize = 10;

    int c;
    
    // command line parsing
    while ((c = getopt(argc, argv, "htnb:")) != -1) {

        switch (c) {

        case 't':
            tFlag = 1;
            break;
        case 'n':
            nFlag = 1;
            break;
        case 'b':
            binSize = atoi( optarg );
            break;
        case 'h':
            printUsage();
            return 0;
        default:
            printUsage();
            return 1;
        }

    }

    // get the info object
    auto_ptr<HistInfo> hist;

    if ( nFlag && tFlag ) {
        hist.reset( new NumTaggedHistInfo( binSize ) );
    }
    else if ( nFlag ) {
        hist.reset( new NumHistInfo( binSize ) );
    }
    else if ( tFlag ) {
        hist.reset( new CatTaggedHistInfo() );
    }
    else {
        hist.reset( new CatHistInfo() );        
    }

    // updating ...
    string line;
    while ( getline( cin, line ) ) {
        hist->update( line );
    }
    
    // displaying ...
    hist->display();

}

void printUsage() {

    cout << "Usage:\t" << "cat dataset | cut -f n | hist " << endl;
    cout << "\t-h\t" << "Help" << endl;
    cout << "\t-n\t" << "Numeric input" << endl;
    cout << "\t-t\t" << "Tag as second column" << endl;
    cout << "\t-b\tbin_size\t" << endl;

}
