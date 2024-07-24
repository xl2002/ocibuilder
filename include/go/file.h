#if !defined(GO_FILE_H)
#define GO_FILE_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <windows.h>
#include <sys/stat.h>

std::string joinPath(const std::string& path1, const std::string& path2);

bool fileExists(const std::string& path);

bool isDirectory(const std::string& path);

bool isRegularFile(const std::string& path);

std::string getDirectory(const std::string& path);
#endif // GO_FILE_H
