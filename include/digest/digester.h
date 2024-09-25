#if !defined(DIGEST_DIGESTER_H)
#define DIGEST_DIGESTER_H

#include "digest/digest.h"

struct Digester_interface{
    // virtual ~Digester_interface() {};
    virtual std::shared_ptr<Hash_256> Hash() = 0;
    virtual std::shared_ptr<Digest> Digest() = 0;
};
struct digester:public Digester_interface{
    std::shared_ptr<Algorithm_sha256> alg;
    std::shared_ptr<Hash_256> hash=std::make_shared<Hash_256>();
    std::shared_ptr<Hash_256> Hash() override;
    // std::shared_ptr<Digest> Digest() override;
};

#endif // DIGEST_DIGESTER_H
