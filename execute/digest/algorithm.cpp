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

std::shared_ptr<Digest> Algorithm_sha256::FromString(const std::string& data) {
    return nullptr;

}