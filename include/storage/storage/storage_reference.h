#if !defined(STORAGE_STORAGE_STORAGE_REFERENCE_H)
#define STORAGE_STORAGE_STORAGE_REFERENCE_H


#include "storage/storage/storage_transport.h"
#include "image/types/reference/reference.h"
#include "storage/storage/images.h"
#include "image/types/define/types.h"
#include "image/types/types.h"
class storageTransport;
// class ImageReference_interface;
struct Image_interface;
struct ImageTransport_interface;
struct SystemContext;
struct ImageSource_interface;
struct ImageDestination_interface;
namespace storage{
    struct Image;
}
class ImageReference_interface{
    public:
    virtual ~ImageReference_interface()=default;
    virtual std::shared_ptr<ImageTransport_interface> Transport() = 0;
    virtual std::string StringWithinTransport() = 0;
    virtual std::string PolicyConfigurationIdentity() = 0;
    virtual std::vector<std::string> PolicyConfigurationNamespaces() = 0;
    virtual std::shared_ptr<Named_interface> DockerReference() = 0;
    virtual std::shared_ptr<Image_interface> NewImage(std::shared_ptr<SystemContext>sys) = 0;
    virtual std::shared_ptr<ImageSource_interface> NewImageSource(std::shared_ptr<SystemContext>sys) = 0;
    virtual std::shared_ptr<ImageDestination_interface> NewImageDestination(std::shared_ptr<SystemContext>sys) = 0;
};
struct storageReference: public ImageReference_interface{
    std::shared_ptr<storageTransport> transport=std::make_shared<storageTransport>();
    std::shared_ptr<Named_interface> named=nullptr;
    std::string id;
    std::shared_ptr<ImageTransport_interface> Transport() override;
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
    std::shared_ptr<storage::Image> resolveImage(std::shared_ptr<SystemContext>sys);
    std::shared_ptr<Image_interface> NewImage(std::shared_ptr<SystemContext>sys) override;
    std::shared_ptr<ImageSource_interface> NewImageSource(std::shared_ptr<SystemContext>sys)override;
    std::shared_ptr<ImageDestination_interface> NewImageDestination(std::shared_ptr<SystemContext>sys)override;
};
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>>ResolveReference(std::shared_ptr<ImageReference_interface>ref);
#endif // STORAGE_STORAGE_REFERENCE_H)
