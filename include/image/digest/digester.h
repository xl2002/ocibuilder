#if !defined(IMAGE_DIGEST_DIGESTER_H)
#define IMAGE_DIGEST_DIGESTER_H

#include "image/digest/digest.h"
class Digest;
class Algorithm_sha256;
struct Digester_interface{
    virtual ~Digester_interface()=default;
    virtual std::shared_ptr<Hash_256> Hash() = 0;
    virtual std::shared_ptr<::Digest> Digest() = 0;
};
struct digester:public Digester_interface{
    std::shared_ptr<Algorithm_sha256> alg;
    std::shared_ptr<Hash_256> hash=nullptr;
    std::shared_ptr<Hash_256> Hash() override;
    std::shared_ptr<::Digest> Digest() override;
};

#endif // DIGEST_DIGESTER_H
