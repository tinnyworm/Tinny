#include <iostream>
#include "ClassA.h"

using namespace std;

int main() {

    cout << "Hello World!" << endl;

    ClassA* a = new ClassA();

    a->methodA();

    a->methodB();

    printf("hollow");
       
}
