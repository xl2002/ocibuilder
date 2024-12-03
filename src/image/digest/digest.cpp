#include "image/digest/digest.h"
#include "utils/common/error.h"
#include "utils/common/regexp.h"
#include <regex>
// DigestRegexp 匹配有效的摘要类型。
const std::regex DigestRegexp("[a-z0-9]+(?:[.+_-][a-z0-9]+)*:[a-zA-Z0-9=_-]+");

// DigestRegexpAnchored 匹配有效的摘要类型，要求匹配从开始到结束。
const std::regex DigestRegexpAnchored("^[a-z0-9]+(?:[.+_-][a-z0-9]+)*:[a-zA-Z0-9=_-]+$");
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
std::shared_ptr<Algorithm_sha256> Digest::Algorithm(){
    auto ret=std::make_shared<Algorithm_sha256>();
    std::string  str(digest.begin(), digest.begin() + sepIndex());
    ret->value=str;
    return ret;
    // return Algorithm(std::string(digest.begin(), digest.begin() + sepIndex()));
}
std::shared_ptr<Verifier_interface> Digest::Verifier() {
    auto ret = std::make_shared<hashVerifier>();
    ret->digest = std::make_shared<Digest>(*this);
    ret->hash=this->Algorithm()->Hash();
    return ret;
}
void Digest::Validate() {
    auto s=this->digest;
    auto i=s.find(':');
    if(i==std::string::npos){
        throw myerror("Digest is not in a valid format");
    }
    if(i<=0 || i+1>=s.size()){
        throw myerror("Digest is not in a valid format");
    }
    auto algorithm=std::make_shared<Algorithm_sha256>(s.substr(0,i));
    auto encoded=s.substr(i+1);
    // 检查算法是否可用
    if (!algorithm->Available()) {
        // 使用正则表达式检查格式
        if (!std::regex_match(s, DigestRegexpAnchored)) {
            throw std::invalid_argument("ErrDigestInvalidFormat");
        }
        throw std::runtime_error("ErrDigestUnsupported");
    }
    
    try{
        algorithm->Validate(encoded);
    }catch(std::exception& e){
        throw myerror(e.what());
    }
}

std::shared_ptr<::Digest> FromBytes(std::vector<uint8_t> p){
    return Canonical_sha256.FromBytes(p);
}
std::shared_ptr<Digest> FromString(std::string s){
    return Canonical_sha256.FromString(s);
}