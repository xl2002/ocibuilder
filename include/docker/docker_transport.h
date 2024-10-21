#if !defined(DOCKER_DOCKER_TRANSPORT_H)
#define DOCKER_DOCKER_TRANSPORT_H
#include "reference/normalize.h"
#include "types/types.h"
class dockerTransport:public ImageTransport_interface{

public:
    std::string Name() override;
    std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference) override;
    void ValidatePolicyConfigurationScope(const std::string &scope) override;
};
extern std::shared_ptr<dockerTransport> docker_Transport;

class dockerReference:public ImageReference_interface{

public:
    std::shared_ptr<Named_interface>ref;
    bool isUnknownDigest=false;
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
    // std::string Name() override;
    std::shared_ptr<ImageTransport_interface> Transport() override;
};

#endif // DOCKER_DOCKER_TRANSPORT_H
