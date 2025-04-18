#include "image/digest/digest.h"
#include "utils/common/error.h"
#include "utils/common/regexp.h"
#include <regex>

/**
 * @brief 获取摘要的编码部分
 * @return std::string 返回摘要的编码部分
 * @throw myerror 当摘要格式无效时抛出
 * @details 从摘要字符串中提取':'后的编码部分
 */
std::string Digest::Encoded() const {
    std::size_t sepindex = sepIndex();
    if (sepindex == std::string::npos || sepindex + 1 >= digest.size()) {
        throw myerror("Digest is not in a valid format");
    }
    return digest.substr(sepindex + 1);
}
/**
 * @brief 查找摘要分隔符位置
 * @return std::size_t 返回分隔符':'的位置索引
 * @details 用于分隔算法名称和编码值的分隔符位置
 */
std::size_t Digest::sepIndex() const {
    // 假设分隔符是 ':'，可以根据实际需要修改
    return digest.find(':');
}
/**
 * @brief 获取摘要的十六进制编码
 * @return std::string 返回十六进制编码字符串
 * @details 等同于Encoded()方法，提供兼容接口
 */
std::string Digest::Hex(){
    return Encoded();
}
/**
 * @brief 获取完整摘要字符串
 * @return std::string 返回完整摘要字符串
 * @details 格式为"算法名:编码值"
 */
std::string Digest::String(){
    return digest;
}
/**
 * @brief 获取摘要使用的算法
 * @return std::shared_ptr<Algorithm_sha256> 返回SHA256算法实例
 * @details 从摘要字符串中解析出算法名称并创建对应算法实例
 */
std::shared_ptr<Algorithm_sha256> Digest::Algorithm(){
    auto ret=std::make_shared<Algorithm_sha256>();
    std::string  str(digest.begin(), digest.begin() + sepIndex());
    ret->value=str;
    return ret;
    // return Algorithm(std::string(digest.begin(), digest.begin() + sepIndex()));
}
/**
 * @brief 创建验证器实例
 * @return std::shared_ptr<Verifier_interface> 返回验证器实例
 * @details 使用当前摘要和对应算法创建哈希验证器
 */
std::shared_ptr<Verifier_interface> Digest::Verifier() {
    auto ret = std::make_shared<hashVerifier>();
    ret->digest = std::make_shared<Digest>(*this);
    ret->hash=this->Algorithm()->Hash();
    return ret;
}
/**
 * @brief 验证摘要格式有效性
 * @throw myerror 当摘要格式无效时抛出
 * @throw std::invalid_argument 当格式不匹配时抛出
 * @throw std::runtime_error 当算法不支持时抛出
 * @details 检查摘要格式、算法可用性和编码有效性
 */
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

/**
 * @brief 从字节数组创建摘要
 * @param p 输入字节数组
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 使用规范SHA256算法从字节数组生成摘要
 */
std::shared_ptr<::Digest> FromBytes(std::vector<uint8_t> p){
    return Canonical_sha256.FromBytes(p);
}
/**
 * @brief 从字符串创建摘要
 * @param s 输入字符串
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 使用规范SHA256算法从字符串生成摘要
 */
std::shared_ptr<Digest> FromString(std::string s){
    return Canonical_sha256.FromString(s);
}
/**
 * @brief 从文件内容创建摘要
 * @param filepath 文件路径
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @throw std::runtime_error 当文件无法打开时抛出
 * @details 计算文件内容的SHA256摘要
 */
std::shared_ptr<Digest> Fromfile(const std::string& filepath){
    return Canonical_sha256.Fromfile(filepath);
}
/**
 * @brief 从算法和哈希创建新摘要
 * @param alg 算法实例
 * @param hash 哈希实例
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 使用算法和哈希数据创建新摘要
 */
std::shared_ptr<Digest> NewDigest(std::shared_ptr<Algorithm_sha256> alg,std::shared_ptr<Hash_256> hash){
    return NewDigestFromBytes(alg,hash->data);
}

/**
 * @brief 从算法和字节数组创建新摘要
 * @param alg 算法实例
 * @param p 字节数组
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 使用算法对字节数组编码后创建摘要
 */
std::shared_ptr<Digest> NewDigestFromBytes(std::shared_ptr<Algorithm_sha256> alg,std::vector<uint8_t> p){
    return NewDigestFromEncoded(alg,alg->Encode(p));
}
/**
 * @brief 从算法和编码字符串创建新摘要
 * @param alg 算法实例
 * @param p 编码字符串
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 组合算法名称和编码值创建摘要字符串
 */
std::shared_ptr<Digest> NewDigestFromEncoded(std::shared_ptr<Algorithm_sha256> alg,std::string p){
    return std::make_shared<Digest>(std::string(alg->String() + ":" + p));
}
