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
};

std::shared_ptr<BlobCache> NewBlobCache(std::shared_ptr<ImageReference_interface> ref, std::string directory, std::shared_ptr<LayerCompression> compress);
#endif // BLOBCACHE_H)
