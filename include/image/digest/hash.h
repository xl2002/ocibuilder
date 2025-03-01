#if !defined(IMAGE_DIGEST_HASH_H)
#define IMAGE_DIGEST_HASH_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
// #include <stdexcept>
// #include <sstream>
// #include <iomanip>
// #include <regex>

// #include "image/digest/sha256.h"
#include <openssl/sha.h>

class Hash_256 {
    public:
    std::shared_ptr<SHA256_CTX> sha256=std::make_shared<SHA256_CTX>();
    std::vector<uint8_t> data;//存计算好的sha256值
    std::vector<uint8_t> Hash_num(const std::vector<uint8_t>& data);

    // 添加 Process 方法
    // void Process(const std::string& input);
};


#endif // GO_HASH_H)
