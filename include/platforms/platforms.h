#if !defined(PLATFORMS_PLATFORMS_H)
#define PLATFORMS_PLATFORMS_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "v1/descriptor.h"

std::shared_ptr<Platform> Normalize(std::shared_ptr<Platform> platform);
namespace platforms {
    std::shared_ptr<Platform> Parse(const std::string& specifier);
}

#endif // PLATFORMS_PLATFORMS_H)