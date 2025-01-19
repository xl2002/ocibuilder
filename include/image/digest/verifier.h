#if !defined(IMAGE_DIGEST_VERIFIER_H)
#define IMAGE_DIGEST_VERIFIER_H
#include "image/digest/digest.h"
#include <string>
#include <vector>
#include <memory>
class Digest;
class Verifier_interface
{
    public:
    virtual ~Verifier_interface() {}
    virtual int write(std::vector<char> p) = 0;
    virtual bool Verified() = 0;
};
class hashVerifier: public Verifier_interface{
    public:
    std::shared_ptr<Digest> digest=std::make_shared<Digest>();
    std::shared_ptr<Hash_256> hash=std::make_shared<Hash_256>();
    int write(std::vector<char> p) override;
    bool Verified() override;
};

#endif // DIGEST_VERIFIER_H)
