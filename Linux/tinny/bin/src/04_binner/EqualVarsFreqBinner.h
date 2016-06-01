#ifndef EQUALVARSFREQBINNER_H
#define EQUALVARSFREQBINNER_H

#include "Binner.h"
#include <string>
#include <vector>

class EqualVarsFreqBinner: public Binner {

public:

    EqualVarsFreqBinner( int bins );
    ~EqualVarsFreqBinner();

    void update( const std::string& strVal );
    void execute();

private:

    int numBins;

    std::vector<double> numArray;

};

#endif
