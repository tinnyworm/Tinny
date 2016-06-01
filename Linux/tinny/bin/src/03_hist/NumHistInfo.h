#ifndef NUMHISTINFO_H
#define NUMHISTINFO_H

#include <map>
#include <string>
#include<boost/regex.hpp>

#include "HistInfo.h"

class NumHistInfo: public HistInfo {

public:

    NumHistInfo( int size );
    ~NumHistInfo();

    void update( const std::string& strVal );
    void display() const;

private:

    long totNum;
    long numBlank;
    long numInvalid;
    
    int binSize;

    const boost::regex blank_pat;
    const boost::regex number_pat;

    std::map<int,long> freqCount;

};

#endif
