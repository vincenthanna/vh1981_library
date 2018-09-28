//https://docs.google.com/document/d/1V2XqjBlWspVGjT35sD8PHWgNj94diwjlKaUkfGFlaA4/edit

#include <iostream>

using namespace std;

class Base{
    public:
        Base() {
            cout << "base " << endl; 
        }

        virtual ~Base() {
            cout << "Base ~" << endl;
        }
};


class Derived : public Base{
    public:
        char* _buf;
        Derived() {
            cout << "derived " << endl; 
            _buf= new char[100];
        }

        ~Derived() {
            cout << "Derived ~" << endl;
            delete[] _buf;
        }
};

class Derived2 : public Derived{
    public:
        char* _buf2;
        Derived2() {
            cout << "derived2 " << endl; 
            _buf2 = new char[100];
        }

        ~Derived2() {
            cout << "Derived2 ~" << endl;
            delete[] _buf2;
        }
};

class Derived3 : public Derived2{
    public:
        char* _buf3;
        Derived3() {
            cout << "derived3 " << endl; 
            _buf3 = new char[100];
        }

        ~Derived3() {
            cout << "Derived3 ~" << endl;
            delete[] _buf3;
        }
};



int main(){
    {
        cout << "---Derived3 as Derived3*:" << endl;
        Derived3* d = new Derived3();
        delete d;
    } 

    {
        cout << "---Derived3 as Base*:" << endl;
        Base* b = new Derived3();
        cout << "test..." << endl;
        delete b;
        cout << "test...2" << endl;
    }

    {
        cout << "---Derived3 as Derived*:" << endl;
        Derived* d = new Derived3();
        delete d;
    }

    {
        cout << "---Derived3 as Derived2*:" << endl;
        Derived2* d = new Derived3();
        delete d;
    }

}

