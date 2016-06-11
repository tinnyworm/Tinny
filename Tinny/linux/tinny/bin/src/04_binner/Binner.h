#ifndef BINNER_H
#define BINNER_H

#include<string>

class Binner {

public:

    virtual ~Binner(){};
    virtual void update( const std::string& strVal ) = 0;
    virtual void execute() = 0;

};

#endif
