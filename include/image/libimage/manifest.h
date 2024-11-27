#if !defined(LIBIMAGE_MANIFEST_H)
#define LIBIMAGE_MANIFEST_H

#include <string>
#include <map>
#include <memory>
#include "storage/storage.h"
#include "types/types.h"
#include "reference/reference.h"
#include "v1/config.h"

using LookupReferenceFunc=function<std::shared_ptr<ImageReference_interface>(std::shared_ptr<ImageReference_interface>)>;
#endif // LIBIMAGE_MANIFEST_H)