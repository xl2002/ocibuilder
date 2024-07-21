#if !defined(UTIL_UTIL_H)
#define UTIL_UTIL_H
#include <string>
#include "cobra/error.h"
#include <fstream>
bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile);
std::string joinPath(const std::string& path1, const std::string& path2);
bool fileExists(const std::string& path);
bool isDirectory(const std::string& path);
bool isRegularFile(const std::string& path);
std::string DiscoverContainerfile(std::string path);
std::string getDirectory(const std::string& path);
std::string resolveSymlinks(const std::string& path);
#endif // UTIL_UTIL_H
