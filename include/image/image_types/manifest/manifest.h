#if !defined(IMAGE_IMAGE_TYPES_MANIFEST_MANIFEST_H)
#define IMAGE_IMAGE_TYPES_MANIFEST_MANIFEST_H
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include "image/digest/digest.h"
#include "bytes/buffer.h"
namespace manifest{
    struct Manifest_interface{
        
    };

    std::shared_ptr<::Digest> Digest(std::vector<byte> manifestBlob);
}

#endif // MANIFEST_MANIFEST_H
