#if !defined(IMAGE_TYPES_SIGNER_H)
#define IMAGE_TYPES_SIGNER_H
#include <string>
#include <vector>
#include <memory>

class SignerImplementation_interface{
    public:
    
    virtual std::string ProgressMessage() = 0;
};
class Signer{
    public:
    std::shared_ptr<SignerImplementation_interface> implementation=nullptr;
};

#endif // SIGNER_SIGNER_H
