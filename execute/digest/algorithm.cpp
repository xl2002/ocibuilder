#include "digest/algorithm.h"

Algorithm_sha256 SHA256("sha256");
Algorithm_sha256 Canonical_sha256=SHA256;
// std::map<Algorithm, std::string (*)(const std::string&)> algorithms = {
//     { SHA256, [](const std::string& data) -> std::string {
//         unsigned char hash[SHA256_DIGEST_LENGTH];
//         SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), hash);
//         std::ostringstream oss;
//         for (const auto& byte : hash) {
//             oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
//         }
//         return oss.str();
//     }}
// };

// 正则表达式映射
std::map<Algorithm_sha256, std::regex> anchoredEncodedRegexps = {
    { SHA256, std::regex("^[a-f0-9]{64}$") }
};
std::map<Algorithm_sha256, std::string> algorithms = {
    { SHA256, "sha256" }
};
std::shared_ptr<Digest> Algorithm_sha256::FromString(const std::string& data) {
    return nullptr;

}
std::shared_ptr<Hash_256> Algorithm_sha256::Hash() {
    return nullptr;
}
bool Algorithm_sha256::Available(){
    if(algorithms.find(*this)!=algorithms.end()){
        return true;
    }
    return false;
}
// Validate 验证编码部分的字符串
void Algorithm_sha256::Validate(const std::string& encoded) {
    auto it = anchoredEncodedRegexps.find(*this);
    
    // 如果没有找到相应的正则表达式
    if (it == anchoredEncodedRegexps.end()) {
        throw std::runtime_error("ErrDigestUnsupported");
    }
    
    const std::regex& r = it->second;
    
    // 摘要必须始终是十六进制编码，确保它的十六进制部分大小总是 size*2
    if (Size() * 2 != encoded.length()) {
        throw std::invalid_argument("ErrDigestInvalidLength");
    }
    
    // 使用正则表达式检查格式
    if (std::regex_match(encoded, r)) {
        return; // 验证成功
    }
    
    throw std::invalid_argument("ErrDigestInvalidFormat");
}
// Size 返回哈希生成的字节数
int Algorithm_sha256::Size() {
    auto it = algorithms.find(*this);
    
    // 如果没有找到对应的算法，返回 0
    if (it == algorithms.end()) {
        return 0;
    }

    // 返回对应算法的大小
    return it->second.size();
}