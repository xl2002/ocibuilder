#if !defined(IMAGE_BUILDAH_PUSH_H)
#define IMAGE_BUILDAH_PUSH_H

#include <string>
#include "image/buildah/buildah.h"
#include "image/types/define/pull.h"
#include "image/types/types.h"
#include "image/libimage/manifest.h"
LookupReferenceFunc cacheLookupReferenceFunc(std::string directory, std::shared_ptr<LayerCompression> compression);
#endif // BUILDAH_PUSH_H)

