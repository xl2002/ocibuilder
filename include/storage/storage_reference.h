#if !defined(STORAGE_STORAGE_REFERENCE_H)
#define STORAGE_STORAGE_REFERENCE_H

#include "types/types.h"
#include "storage/storage_transport.h"
#include "reference/reference.h"
#include "storage/images.h"
#include "define/types.h"
class storageTransport;
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
};
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<storage::Image>>ResolveReference(std::shared_ptr<ImageReference_interface>ref);
#endif // STORAGE_STORAGE_REFERENCE_H)
