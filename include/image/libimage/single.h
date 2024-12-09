#if !defined(IMAGE_LIBIMAGE_SINGLE_H)
#define IMAGE_LIBIMAGE_SINGLE_H
#include "image/digest/algorithm.h"
// #include "image/types/copy.h"
#include "image/types/internal/types.h"

struct copySingleImageOptions{
    bool requireCompressionFormatMatch=false;
    std::shared_ptr<Algorithm> compressionFormat=std::make_shared<Algorithm>();
    std::shared_ptr<int> compressionLevel=std::make_shared<int>(0);
    copySingleImageOptions()=default;
};
struct copySingleImageResult{
    std::vector<uint8_t> manifest;
    std::string manifestMIMEType;
    std::shared_ptr<Digest> manifestDigest=std::make_shared<Digest>();
    std::vector<std::shared_ptr<Algorithm>> compressionAlgorithms;
};
#endif // IMAGE_LIBIMAGE_SINGLE_H)
