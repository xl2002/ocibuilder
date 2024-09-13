#if !defined(GITHUB_DIGEST_H)
#define GITHUB_DIGEST_H
#include <string>
#include <vector>
#include "go/hash.h"
#include "digest/algorithm.h"
using std::string;
using std::vector;

class Digest{
    public:
    string digest;
    Digest()=default;
    Digest(string d):digest(d){};
    std::string Encoded() const;
    std::size_t sepIndex() const;
    std::string Hex();
    std::string String();
};


#endif // GITHUB_DIGEST_H
