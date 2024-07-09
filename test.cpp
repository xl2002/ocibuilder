#include <iostream>
#include <string>
#include <array>
#include <tuple>
// #include "flag.h"
using namespace std;

class test{
	public:
	virtual tuple<string,bool> print()=0;
};
class Print:public test{
	public:
	int n;
	Print(int n):n(n){};
	tuple<string,bool> print() override{
		cout<<"hello 纯虚函数"<<endl;
		return make_tuple(string("hello"),true);
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
	auto [ret,Bool]= var.t->print();
	return 0;
}
