#include <iostream>
#include <string>
#include <array>
// #include "flag.h"
using namespace std;

class test{
	public:
	virtual void print()=0;
};
class Print:public test{
	public:
	int n;
	Print(int n):n(n){};
	void print() override{
		cout<<"hello 纯虚函数"<<endl;
	};
};

class a{
	public:
	test* t;
	a(test *p):t(p){};
};

int main(int argc, char const *argv[])
{
	/* code */
	Print p(3);
	a var(&p);
	var.t->print();
	return 0;
}
