#ifndef CATTAGGEDHISTINFO_H
#define CATTAGGEDHISTINFO_H

#include <map>
#include <string>
#include<boost/regex.hpp>

#include "HistInfo.h"

class CatTaggedHistInfo: public HistInfo {

public:

    CatTaggedHistInfo();
    ~CatTaggedHistInfo();

    void update( const std::string& strValo );
    void display() const;

private:

    long totNum;

    const boost::regex blank_pat;
    const boost::regex bad_pat;

    std::map<std::string,long> allCount;
    std::map<std::string,long> badCount;

};

#endif
