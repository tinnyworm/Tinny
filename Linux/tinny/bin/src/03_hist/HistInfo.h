#ifndef HISTINFO_H
#define HISTINFO_H

#include <string>

class HistInfo {

public:

    virtual ~HistInfo(){};
    virtual void update( const std::string& strVal ) = 0;
    virtual void display() const = 0;

};

#endif
