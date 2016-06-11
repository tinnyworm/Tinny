#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

#include "stat_math.h"

int main (int argc, char * argv[])
{
    if (argc!=3) {
        cerr << "Usage: chi_squared degrees_of_freedom argument" << endl;
        exit(1);
    }
    cout << chi_squared(atoi(argv[1]), atof(argv[2])) << endl;
}
