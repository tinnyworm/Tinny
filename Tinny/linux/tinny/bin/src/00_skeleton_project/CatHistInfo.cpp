#include<iostream>
#include <iomanip>
#include<string>
#include<map>
#include<boost/regex.hpp>

#include "CatHistInfo.h"

using namespace std;
//using namespace boost;

CatHistInfo::CatHistInfo(): totNum(0) {}

CatHistInfo::~CatHistInfo(){}

void CatHistInfo::update(){

    cout << "update record ..." << endl;

}

void CatHistInfo::display(){

    cout << "display ..." << endl;

}
