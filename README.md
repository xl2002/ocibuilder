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
g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/gzip_test.exe ./tests/gzip_test.cpp -L lib\boost-MinGW -L lib\zlib-MinGW -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lz

g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/sha256_test.exe ./tests/sha256_test.cpp -L lib\boost-MinGW -L lib\openssl -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lboost_system-mgw12-mt-x64-1_75 -lboost_thread-mgw12-mt-x64-1_75 -lws2_32 -lboost_json-mgw12-mt-x64-1_75 -lssl -lcrypto

g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/sha256_test.exe ./tests/sha256_test.cpp -L lib\boost-MinGW -L lib\openssl -L lib\libarchive -lboost_filesystem-mgw12-mt-x64-1_75 -lssl -lcrypto -larchive -lbcrypt

g++ -std=c++11 -Wall -Wextra -g -I include sha256_test.cpp -o sha256_test  -L lib -lssl -lcrypto

编译opanssl库
sudo apt-get install mingw-w64
wget https://www.openssl.org/source/openssl-1.1.1w.tar.gz
tar -xzvf openssl-1.1.1w.tar.gz
cd openssl-1.1.1w
./Configure mingw64 no-shared no-dso no-tests --cross-compile-prefix=x86_64-w64-mingw32-
make

linux挤压镜像层测试
mv d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24 d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar.gz
gunzip d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar.gz
shasum -a 256 d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar