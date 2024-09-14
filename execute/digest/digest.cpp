#include "digest/digest.h"
#include "cobra/error.h"
// 返回编码部分的方法
std::string Digest::Encoded() const {
    std::size_t sepindex = sepIndex();
    if (sepindex == std::string::npos || sepindex + 1 >= digest.size()) {
        throw myerror("Digest is not in a valid format");
    }
    return digest.substr(sepindex + 1);
}
// 查找分隔符的位置
std::size_t Digest::sepIndex() const {
    // 假设分隔符是 ':'，可以根据实际需要修改
    return digest.find(':');
}
std::string Digest::Hex(){
    return Encoded();
}
std::string Digest::String(){
    return digest;
}
