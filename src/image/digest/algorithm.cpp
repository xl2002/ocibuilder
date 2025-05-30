#include "image/digest/algorithm.h"
#include "utils/logger/ProcessSafeLogger.h"
#include <sstream>
#include <iomanip>
#include <fstream>

std::map<Algorithm_sha256, std::string> algorithms = {
    { sha_256, "sha256" }
};
/**
 * @brief 将输入的字符串data计算出对应的哈希，返回Digest指针
 * 
 * @param data 用于计算的字符串
 * @return std::shared_ptr<Digest> 返回对应的Digest指针
 */
std::shared_ptr<Digest> Algorithm_sha256::FromString(const std::string& data) {
    return this->FromBytes(std::vector<uint8_t>(data.begin(), data.end()));
}
/**
 * @brief 将输入的数据p计算出对应的哈希，返回Digest指针
 * 
 * @param p 用于计算的数据p
 * @return std::shared_ptr<Digest> 返回对应的Digest指针
 */
std::shared_ptr<Digest> Algorithm_sha256::FromBytes(std::vector<uint8_t> p){
    auto digester = this->Digester();
    digester->GetHash()->data=digester->GetHash()->Hash_num(p);

    return digester->Digest();
}
/**
 * @brief 新建一个Hash_256
 * 
 * @return std::shared_ptr<Hash_256> 
 */
std::shared_ptr<Hash_256> Algorithm_sha256::Hash() {
    auto hash_256=new Hash_256();
    SHA256_Init(hash_256->sha256.get());
    return std::shared_ptr<Hash_256>(hash_256);
}
/**
 * @brief 检查当前算法是否可用
 * @return bool 返回算法是否可用
 * @details 通过检查算法映射表中是否存在当前算法来判断可用性
 */
bool Algorithm_sha256::Available(){
    if(algorithms.find(*this)!=algorithms.end()){
        return true;
    }
    return false;
}
/**
 * @brief 验证编码字符串格式
 * @param encoded 待验证的编码字符串
 * @throw std::runtime_error 当算法不支持时抛出
 * @throw std::invalid_argument 当长度或格式不匹配时抛出
 * @details 检查编码字符串是否符合算法要求的长度和正则表达式格式
 */
void Algorithm_sha256::Validate(const std::string& encoded) {
    auto it = anchoredEncodedRegexps.find(*this);
    
    // 如果没有找到相应的正则表达式
    if (it == anchoredEncodedRegexps.end()) {
        logger->log_error("ErrDigestUnsupported");
        throw std::runtime_error("ErrDigestUnsupported");
    }
    
    const std::regex& r = it->second;
    
    // 摘要必须始终是十六进制编码，确保它的十六进制部分大小总是 size*2
    if (Size() * 2 != encoded.length()) {
        logger->log_error("ErrDigestInvalidLength");
        throw std::invalid_argument("ErrDigestInvalidLength");
    }
    
    // 使用正则表达式检查格式
    if (std::regex_match(encoded, r)) {
        return; // 验证成功
    }
    logger->log_error("ErrDigestInvalidFormat");
    throw std::invalid_argument("ErrDigestInvalidFormat");
}
/**
 * @brief 获取哈希值的字节长度
 * @return int 返回哈希值的字节长度
 * @details 根据当前算法类型返回对应的哈希值长度
 */
int Algorithm_sha256::Size() {
    auto it = algorithms.find(*this);
    
    // 如果没有找到对应的算法，返回 0
    if (it == algorithms.end()) {
        return 0;
    }

    // 返回对应算法的大小
    return it->second.size();
}
/**
 * @brief 将输入的digest值p转换为string
 * 
 * @param p 输入的digest值
 * @return std::string 返回对应的string
 */
std::string Algorithm_sha256::Encode(std::vector<uint8_t> p){
    std::stringstream ss;

    // 遍历每个字节，并将其转换为两位的十六进制字符串
    for (auto byte : p) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
    }
    std::string ret=ss.str();
    return ret;
}
/**
 * @brief 构建一个新的digester实例
 * 
 * @return std::shared_ptr<Digester_interface> 返回新的digester实例指针
 */
std::shared_ptr<Digester_interface> Algorithm_sha256::Digester(){
    auto hash = this->Hash();

    // 构造 digester 实例，将当前算法（this）和新创建的 hash 实例传入
    auto newDigester = std::make_shared<digester>();
    newDigester->alg = std::make_shared<Algorithm_sha256>(*this);
    newDigester->hash = hash;
    // auto newDigester = std::make_shared<digester>(std::make_shared<Algorithm_sha256>(*this), hash);

    return newDigester;
    
}

/**
 * @brief 获取算法名称字符串
 * @return std::string 返回算法名称
 * @details 返回当前算法的字符串表示形式
 */
std::string Algorithm_sha256::String(){
    return this->value;
}
std::shared_ptr<Digest> Algorithm_sha256::Fromfile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        logger->log_error("can not open file:"+filepath);
        throw std::runtime_error("无法打开文件: " + filepath);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    // 处理最后剩余的数据
    if (file.gcount() > 0) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    // 转换为十六进制字符串
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    file.close();
    // return oss.str();
    auto digest=std::make_shared<Digest>();
    digest->digest="sha256:"+oss.str();
    return digest;
}
