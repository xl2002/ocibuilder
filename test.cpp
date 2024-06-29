#include <iostream>
#include <string>
#include <array>
using namespace std;

class Student{
	private:
		int age;
		string name;
		string sex;
	public:
		// 定义内置函数
		inline void set_value(){
			cin >>name >>age >>sex;
		}
		// 显示成员信息
		void display(){
			cout <<endl;
			cout <<"name:" <<name <<", age:" <<age <<", sex:" <<sex <<endl;
		}
		virtual string get_name();
};

int main(){
	Student s;
	s.set_value();
	s.display();
	// array(3);
	string student_name = s.get_name();
	cout <<"student name:" <<student_name <<endl;
}

string Student::get_name(){
	return name;
}