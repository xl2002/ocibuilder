# ocibuilder
oci规范的镜像构建工具，适用于windows系统




注释添加：添加函数中文描述，函数代码绝对不能更改
        添加类或者结构体中文描述，函数代码绝对不能更改
        将go语言代码翻译为C++11标准的C++代码
任务：将go语言代码翻译成对应的C++11规范的C++代码，并且同时满足以下所有要求。
要求：1.翻译结果的C++代码的变量名，类名需要保持一致
2.对于go语言代码中的英文注释，在C++代码中对应翻译成中文
3.C++代码只允许使用标准stl库和boost库,首先考虑标准库，boost库版本为1_75_0
4.如果C++代码对于linux和window系统实现不同，则用编译宏给出跨平台写法
5.C++代码不用给出main测试代码
6.错误处理时，采用抛出的方法，不使用boost::system::error_code

test.cpp 运行
g++ -std=c++11 -Wall -Wextra -g -I include -o test.exe test.cpp -L lib\lib64-MinGW -lboost_filesystem-mgw12-mt-x64-1_75