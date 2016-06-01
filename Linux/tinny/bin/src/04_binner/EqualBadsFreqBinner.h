#ifndef EQUALBADSFREQBINNER_H
#define EQUALBADSFREQBINNER_H

#include "Binner.h"

#include <string>
#include <map>
#include <boost/regex.hpp>

class EqualBadsFreqBinner: public Binner {

public:

    EqualBadsFreqBinner( int bins );
    ~EqualBadsFreqBinner();

    void update( const std::string& strVal );
    void execute();

private:

    int numBads;
    int numBins;

    const boost::regex bad_pat;
    std::multimap<double, std::string> numMap;

};

#endif
