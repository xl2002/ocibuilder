#if !defined(SIGNER_SIGNER_H)
#define SIGNER_SIGNER_H
#include <string>
#include <vector>
#include <memory>

class SignerImplementation_interface{
    public:
    virtual std::string ProgressMessage() = 0;
};
struct Signer{
    std::shared_ptr<SignerImplementation_interface> implementation=nullptr;
};

#endif // SIGNER_SIGNER_H
