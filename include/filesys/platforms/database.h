#if !defined(FILESYS_PLATFORMS_DATABASE_H)
#define FILESYS_PLATFORMS_DATABASE_H
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>
#include <tuple>
bool isKnownOS(const std::string& os);
bool isArmArch(const std::string& arch);
bool isKnownArch(const std::string& arch);
std::string normalizeOS(const std::string& os);
std::tuple<std::string, std::string> normalizeArch(std::string arch, std::string variant);
#endif // PLATFORMS_DATABASE_H)
