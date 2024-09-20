#if !defined(DIGEST_VERIFIER_H)
#define DIGEST_VERIFIER_H
#include "digest/digest.h"
#include <string>
#include <vector>
#include <memory>
class Digest;
struct Verifier_interface
{
    virtual ~Verifier_interface() {}
    virtual int write(std::vector<char> p) = 0;
    virtual bool Verified() = 0;
};
struct hashVerifier: public Verifier_interface{
    std::shared_ptr<Digest> digest=nullptr;
    std::shared_ptr<Hash_256> hash=nullptr;
    int write(std::vector<char> p) override;
    bool Verified() override;
};

#endif // DIGEST_VERIFIER_H)
