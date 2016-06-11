#include <iostream>

class Rose {};
class Cow {};

class A { public: typedef Rose rose; };

template<typename T>
class B : public T { public: typedef typename T::rose foo; };

template<typename T>
void smell( T ) {std::cout << "awful" << std::endl;}

void smell( Rose ) { std::cout << "sweet" << std::endl; }

int main()
{

    smell( A::rose() );
    smell( B<A>::foo() );
    smell( Cow() );

}
