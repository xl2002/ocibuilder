#if !defined(IMAGE_BUILDAH_UTIL_H)
#define IMAGE_BUILDAH_UTIL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "image/types/define/types.h"
#include "storage/storage.h"
#include "image/types/types.h"

bool isReferenceBlocked(std::shared_ptr<ImageReference_interface> ref,std::shared_ptr<SystemContext> sc);
bool isReferenceSomething(
    std::shared_ptr<ImageReference_interface> ref,
    std::shared_ptr<SystemContext> sc,
    std::function<bool(std::shared_ptr<ImageReference_interface>,std::shared_ptr<SystemContext>)> func
);
std::map<std::string,std::string> copyStringStringMap(std::map<std::string,std::string> src);
#endif // BUILDAH_UTIL_H
