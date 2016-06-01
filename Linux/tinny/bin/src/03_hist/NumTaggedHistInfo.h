#ifndef NUMTAGGEDHISTINFO_H
#define NUMTAGGEDHISTINFO_H

#include <map>
#include <string>
#include<boost/regex.hpp>

#include "HistInfo.h"

class NumTaggedHistInfo: public HistInfo {

public:

    NumTaggedHistInfo( int size );
    ~NumTaggedHistInfo();

    void update( const std::string& strVal );
    void display() const;

private:

    long totNum;
    long numBlank;
    long numBadBlank;
    long numInvalid;
    long numBadInvalid;
    
    int binSize;

    const boost::regex blank_pat;
    const boost::regex number_pat;
    const boost::regex bad_pat;

    std::map<int,long> allCount;
    std::map<int,long> badCount;

};

#endif
