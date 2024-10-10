#if !defined(PLATFORMS_PLATFORMS_H)
#define PLATFORMS_PLATFORMS_H
#include "v1/descriptor.h"
#include "platforms/database.h"
#include "platforms/cpuinfo.h"
Platform Normalize(const Platform& platform);
// Parse 函数
Platform Parse(const std::string& specifier);
#endif