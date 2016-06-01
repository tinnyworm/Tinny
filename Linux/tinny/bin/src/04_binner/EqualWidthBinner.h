#ifndef EQUALWIDTHBINNER_H
#define EQUALWIDTHBINNER_H

#include "Binner.h"
#include <string>

class EqualWidthBinner: public Binner {

public:

    EqualWidthBinner( int bins );
    ~EqualWidthBinner();

    void update( const std::string& strVal );
    void execute();

private:

    int numBins;

    double min;
    double max;

};

#endif
