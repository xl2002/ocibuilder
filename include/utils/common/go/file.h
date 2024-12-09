#if !defined(UTILS_COMMON_GO_FILE_H)
#define UTILS_COMMON_GO_FILE_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <winsock2.h>
#include <windows.h>
#include <sys/stat.h>
#include <fstream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

std::string joinPath(const std::string& path1, const std::string& path2);

bool fileExists(const std::string& path);

bool isDirectory(const std::string& path);

bool isRegularFile(const std::string& path);

std::string getDirectory(const std::string& path);

void  WriteFile(const std::string& iidfile, const std::string& imageID);

int64_t Copy_directory(const fs::path& source, const fs::path& destination);
#endif // GO_FILE_H