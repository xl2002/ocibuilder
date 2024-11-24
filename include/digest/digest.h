#if !defined(GITHUB_DIGEST_H)
#define GITHUB_DIGEST_H
#include <string>
#include <vector>
#include "go/hash.h"
#include "digest/algorithm.h"
#include "digest/verifier.h"
using std::string;
using std::vector;
class Algorithm_sha256;
struct Verifier_interface;
class Digest{
    public:
    string digest;
    Digest()=default;
    Digest(string d):digest(d){};
    std::string Encoded() const;
    std::size_t sepIndex() const;
    std::string Hex();
    std::string String();
    std::shared_ptr<Algorithm_sha256> Algorithm();
    std::shared_ptr<Verifier_interface> Verifier();
    void Validate();
    bool operator< (const Digest& other) const {
        return digest < other.digest;
    }
};


#endif // GITHUB_DIGEST_H
