#if !defined(IMAGE_IMAGE_TYPES_DOCKER_DOCKER_TRANSPORT_H)
#define IMAGE_IMAGE_TYPES_DOCKER_DOCKER_TRANSPORT_H
#include "image/types/reference/normalize.h"
#include "image/types/types.h"
class dockerTransport:public ImageTransport_interface{

public:
    std::string Name() override;
    std::shared_ptr<ImageReference_interface> ParseReference(const std::string &reference) override;
    void ValidatePolicyConfigurationScope(const std::string &scope) override;
};
extern std::shared_ptr<dockerTransport> docker_Transport;

class dockerReference:public ImageReference_interface{
public:
    std::shared_ptr<Named_interface>ref=nullptr;
    bool isUnknownDigest=false;
    dockerReference()=default;
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
    // std::string Name() override;
    std::shared_ptr<ImageTransport_interface> Transport() override;
    std::shared_ptr<Image_interface> NewImage(std::shared_ptr<SystemContext>sys) override;
    std::shared_ptr<ImageSource_interface> NewImageSource(std::shared_ptr<SystemContext>sys,bool check) override;
    std::shared_ptr<ImageDestination_interface> NewImageDestination(std::shared_ptr<SystemContext>sys) override;
};
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> ParseReference(std::string refString);
#endif // DOCKER_DOCKER_TRANSPORT_H
