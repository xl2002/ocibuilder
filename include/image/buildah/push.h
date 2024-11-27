#if !defined(BUILDAH_PUSH_H)
#define BUILDAH_PUSH_H

#include <string>
#include "buildah/buildah.h"
#include "define/pull.h"
#include "types/types.h"
#include "libimage/manifest.h"
LookupReferenceFunc cacheLookupReferenceFunc(std::string directory, std::shared_ptr<LayerCompression> compression);
#endif // BUILDAH_PUSH_H)

