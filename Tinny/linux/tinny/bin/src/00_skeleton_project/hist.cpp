#include<iostream>

#include "CatHistInfo.h"

using namespace std;

int main() {

    HistInfo* hist = new CatHistInfo();

    hist->update();

    hist->display();

    delete hist;

}
