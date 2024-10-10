#if !defined(PLATFORMS_DATABASE_H)
#define PLATFORMS_DATABASE_H
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>

std::string normalizeOS(const std::string& os);
std::pair<std::string, std::string> normalizeArch(const std::string& arch, const std::string& variant);
bool isKnownOS(const std::string& os);
bool isKnownArch(const std::string& arch);
std::string GetWindowsOsVersion();
// 判断当前架构是否为ARM
bool isArmArch(const std::string& arch);
#endif