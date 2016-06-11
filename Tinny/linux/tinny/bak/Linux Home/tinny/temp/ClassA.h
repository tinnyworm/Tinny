#include <iostream>

class ClassA {

 public:

    ClassA() {
	a1 = 0;
	a2 = 0;
    }

    ~ClassA() {}

    void methodA() {
	std::cout << "Method A Called" << std::endl;
    }

    void methodB() {
	std::cout << "Method B Called" << std::endl;
    }

 private:

    int a1;
   
    int a2;

};
