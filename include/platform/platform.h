#if !defined(PLATFORM_PLATFORM_H)
#define PLATFORM_PLATFORM_H
#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp> // 用于字符串处理
#include "v1/descriptor.h"
#include "platforms/platforms.h"
Platform NormalizePlatform(const std::string& rawOS, const std::string& rawArch, const std::string& rawVariant);
std::string ToString(const std::string& os, const std::string& arch, const std::string& variant);

#endif