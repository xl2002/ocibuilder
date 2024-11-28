#if !defined(FILESYS_PLATFORMS_PLATFORMS_H)
#define FILESYS_PLATFORMS_PLATFORMS_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "image/image_types/v1/descriptor.h"

std::shared_ptr<Platform> Normalize(std::shared_ptr<Platform> platform);
namespace PlatForms {
    std::shared_ptr<Platform> Parse(const std::string& specifier);
}

#endif // PLATFORMS_PLATFORMS_H)