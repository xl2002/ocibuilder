#if !defined(GO_HASH_H)
#define GO_HASH_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
// #include <stdexcept>
// #include <sstream>
// #include <iomanip>
// #include <regex>

#include "go/sha256.h"

struct Hash_256 {
    std::shared_ptr<SHA256_CTX> sha256=std::make_shared<SHA256_CTX>();

};


#endif // GO_HASH_H)
