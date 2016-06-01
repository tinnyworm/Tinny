#ifndef CATHISTINFO_H
#define CATHISTINFO_H

#include <map>
#include <string>

#include "HistInfo.h"

class CatHistInfo: public HistInfo {

public:

    CatHistInfo();
    ~CatHistInfo();

    void update();
    void display();

private:

    long totNum;
    std::map<std::string,long> freqCount;

};

#endif
