#if !defined(IMAGE_DIGEST_DIGESTER_H)
#define IMAGE_DIGEST_DIGESTER_H

#include "image/digest/digest.h"
class Digest;
class Algorithm_sha256;
struct Digester_interface{
    virtual ~Digester_interface()=default;
    virtual std::shared_ptr<Hash_256> GetHash() = 0;
    virtual std::shared_ptr<::Digest> Digest() = 0;
    virtual std::shared_ptr<Algorithm_sha256> SetAlgorithm() = 0;
    
};
struct digester:public Digester_interface{
    std::shared_ptr<Algorithm_sha256> alg;
    std::shared_ptr<Hash_256> hash=nullptr;
    digester(std::shared_ptr<Algorithm_sha256> algorithm = nullptr, std::shared_ptr<Hash_256> hashData = nullptr);
    //Getter for hash
    std::shared_ptr<Hash_256> GetHash() override;
    // Getter for alg
    std::shared_ptr<Algorithm_sha256> GetAlgorithm() const;
    // Setter for alg
    void SetAlgorithm(const std::shared_ptr<Algorithm_sha256>& algorithm);


    std::shared_ptr<::Digest> Digest() override;
};

#endif // DIGEST_DIGESTER_H
