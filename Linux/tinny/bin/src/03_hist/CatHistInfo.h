#ifndef CATHISTINFO_H
#define CATHISTINFO_H

#include <map>
#include <string>
#include<boost/regex.hpp>

#include "HistInfo.h"

class CatHistInfo: public HistInfo {

public:

    CatHistInfo();
    ~CatHistInfo();

    void update( const std::string& strValo );
    void display() const;

private:

    long totNum;

    const boost::regex blank_pat;

    std::map<std::string,long> freqCount;

};

#endif
