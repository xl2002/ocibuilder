#if !defined(DIGEST_ALGORITHM_H)
#define DIGEST_ALGORITHM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <regex>
#include "digest/digest.h"
class Digest;
struct Algorithm_sha256 {
    std::string value;
    Algorithm_sha256() = default;
    Algorithm_sha256(std::string v) : value(v) {};
    std::shared_ptr<Digest> FromString(const std::string& data);
    bool operator< (const Algorithm_sha256& other) const {
        return value < other.value;
    }
};
extern  Algorithm_sha256 SHA256;
extern  Algorithm_sha256 Canonical_sha256;
extern std::map<Algorithm_sha256, std::regex> anchoredEncodedRegexps;

#endif // DIGEST_ALGORITHM_H)
