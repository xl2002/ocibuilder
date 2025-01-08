#include "image/image_types/manifest/manifest.h"

std::shared_ptr<::Digest> manifest::Digest(std::vector<byte> manifestBlob){

    return FromBytes(manifestBlob);
}
