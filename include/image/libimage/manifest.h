#if !defined(IMAGE_LIBIMAGE_MANIFEST_H)
#define IMAGE_LIBIMAGE_MANIFEST_H

#include <string>
#include <map>
#include <memory>
#include "storage/storage.h"
#include "image/types/types.h"
#include "image/types/reference/reference.h"
#include "image/image_types/v1/config.h"

using LookupReferenceFunc=function<std::shared_ptr<ImageReference_interface>(std::shared_ptr<ImageReference_interface>)>;
#endif // LIBIMAGE_MANIFEST_H)