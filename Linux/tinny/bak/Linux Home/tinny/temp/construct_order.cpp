#include <iostream>

class B {

public:
    
    B() {
        std::cout << "Executing B::B()\n";
    }

    B(int val) {
        b = 0;
        std::cout << "Executing B::B(int)\n";
    }

    virtual ~B() { 
        std::cout << "Executing B::~B()\n";
    }

private:

    int b;

};

class D1 : virtual public B {

public:

    D1() { 
        std::cout << "Executing D1::D1()\n";
    }

    D1(int val) {
        d1 = val;
        std::cout << "Executing D1::D1(int)\n";
    }

    ~D1() { 
        std::cout << "Executing D1::~D1()\n";
    }

private:

    int d1;

};


class D2 : virtual public B {

public:

    D2() { 
        std::cout << "Executing D2::D2()\n";
    }

    D2(int val) {
        d1 = val;
        std::cout << "Executing D2::D2(int)\n";
    }

    ~D2() { 
        std::cout << "Executing D2::~D2()\n";
    }

private:

    int d1;

};


class MD : public D1, public D2 {

public:
    
    MD() { 
        std::cout << "Executing MD::MD()\n";
    }

    MD(int val) { 
        md = val;
        std::cout << "Executing MD::MD(int)\n";
    }

    ~MD() { 
        std::cout << "Executing MD::~MD()\n";
    }

private:

    int md;

};

int main()
{

    B b(1);

    std::cout << "\n\n";

    D1 d1(1);

    std::cout << "\n\n";

    D2 d2(2);

    std::cout << "\n\n";

    MD md(4);

    std::cout << "\n\n";

}
