# ocibuilder
oci规范的镜像构建工具，适用于windows系统

### 工具使用说明

1. 安装MinGW运行环境
2. 运行mingw32-make，编译代码，main.exe文件位于./outpout下
3. 运行main.exe前，确保同目录下存在Dockerfile文件，确保Dockerfile中文件夹路径有效
~~~dockerfile
#from 后面路径为镜像需要的组件的路径，可以采用绝对路径，相对路径注意是相对main的运行路径，目录下的内容在镜像的根目录下
FROM ./baseimage
# copy 将源路径的内容打包成镜像层，位于镜像的/usr/src/app目录下
COPY ./app /usr/src/app
~~~

4. 目前main支持的参数

~~~bash
#根据Dockerfile构建busybox-image:latest镜像
./main.exe build --tag busybox-image:latest .
#推送镜像，localhost:5000为网络地址和端口，busybox-image:latest为镜像名
./main.exe push localhost:5000/busybox-image:latest
~~~



chatgpt提示工程

~~~
任务：将go语言代码翻译成对应的C++11规范的C++代码，并且同时满足以下所有要求。
要求：1.翻译结果的C++代码的变量名，类名需要保持一致
2.对于go语言代码中的英文注释，在C++代码中对应翻译成中文
3.C++代码只允许使用标准stl库和boost库,首先考虑标准库，boost库版本为1_75_0
4.如果C++代码对于linux和window系统实现不同，则用编译宏给出跨平台写法
5.C++代码不用给出main测试代码
6.错误处理时，采用抛出的方法，不使用boost::system::error_code
~~~

test.cpp 运行

~~~bash
g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/gzip_test.exe ./tests/gzip_test.cpp -L lib\boost-MinGW -L lib\zlib-MinGW -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lz

g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/sha256_test.exe ./tests/sha256_test.cpp -L lib\boost-MinGW -L lib\openssl -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lboost_system-mgw12-mt-x64-1_75 -lboost_thread-mgw12-mt-x64-1_75 -lws2_32 -lboost_json-mgw12-mt-x64-1_75 -lssl -lcrypto

g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/sha256_test.exe ./tests/sha256_test.cpp -L lib\boost-MinGW -L lib\openssl -L lib\libarchive -lboost_filesystem-mgw12-mt-x64-1_75 -lssl -lcrypto -larchive -lbcrypt

g++ -std=c++11 -Wall -Wextra -g -I include sha256_test.cpp -o sha256_test  -L lib -lssl -lcrypto



g++ -std=c++11 -Wall -Wextra -g -I include -o ./tests/pull_test.exe ./tests/pull_test.cpp -L lib\boost -L lib\openssl  -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lboost_system-mgw12-mt-x64-1_75 -lboost_thread-mgw12-mt-x64-1_75 -lws2_32 -lboost_json-mgw12-mt-x64-1_75 -lssl -lcrypto
~~~

编译opanssl库

~~~bash
sudo apt-get install mingw-w64
wget https://www.openssl.org/source/openssl-1.1.1w.tar.gz
tar -xzvf openssl-1.1.1w.tar.gz
cd openssl-1.1.1w
./Configure mingw64 no-shared no-dso no-tests --cross-compile-prefix=x86_64-w64-mingw32-
make
~~~

linux解压镜像层测试

~~~bash
mv d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24 d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar.gz
gunzip d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar.gz
shasum -a 256 d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24.tar
~~~

./output/main.exe build --tag image1:latest .
./output/main.exe build --annotation "version=3.0" --annotation "author=NWPU" --arch amd64 --os linux --file Dockerfile --tag busybox-image:latest .
./output/main.exe push 10.68.1.145:5000/busybox-image:latest
./output/main.exe version
./output/main.exe tag image1:latest image2:latest
./output/main.exe rmi 362a1d5145be