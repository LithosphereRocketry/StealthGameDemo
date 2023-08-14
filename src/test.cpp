#include <iostream>
using namespace std;

class Foo { public: virtual void baz() = 0; };

class Bar: public virtual Foo {};

class Foo1: public virtual Foo {
    public: virtual void baz() {
        cout << "1\n";
    } 
};

class Foo2: public virtual Foo {
    public: virtual void baz() {
        cout << "2\n";
    }
};

class Bar1: public Bar, public Foo1 {};
class Bar2: public Bar, public Foo2 {};

int main() {
    Bar* list[] = {
        new Bar1(),
        new Bar2(),
        new Bar1()
    };

    for(int i = 0; i < 3; i++) {
        list[i]->baz();
    }
}