#if !defined(IMAGE_TYPES_BLOBCACHE_H)
#define IMAGE_TYPES_BLOBCACHE_H
#include "image/types/types.h"
#include "image/types/reference/reference.h"

struct BlobCache_interface:public ImageReference_interface{
    virtual std::string Directory()=0;
};

struct BlobCache:public BlobCache_interface
{
    std::shared_ptr<ImageReference_interface> ref=nullptr;
    std::string directory;
    std::shared_ptr<LayerCompression> compress=std::make_shared<LayerCompression>();
    std::string StringWithinTransport() override;
    std::string PolicyConfigurationIdentity() override;
    std::vector<std::string> PolicyConfigurationNamespaces() override;
    std::shared_ptr<Named_interface> DockerReference() override;
    // std::string Name() override;
    std::shared_ptr<ImageTransport_interface> Transport() override;
    std::shared_ptr<Image_interface> NewImage(std::shared_ptr<SystemContext>sys) override;
    std::shared_ptr<ImageSource_interface> NewImageSource(std::shared_ptr<SystemContext>sys,bool check) override;
    std::shared_ptr<ImageDestination_interface> NewImageDestination(std::shared_ptr<SystemContext>sys) override;
    std::string Directory() override;
};

std::shared_ptr<BlobCache> NewBlobCache(std::shared_ptr<ImageReference_interface> ref, std::string directory, std::shared_ptr<LayerCompression> compress);
#endif // BLOBCACHE_H)
