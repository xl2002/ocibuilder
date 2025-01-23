#if !defined(UTILS_COMMON_GO_FILE_H)
#define UTILS_COMMON_GO_FILE_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "filesys/systems.h"
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
using namespace std;
extern char Separator;
std::string joinPath(const std::string& path1, const std::string& path2);

bool fileExists(const std::string& path);

bool isDirectory(const std::string& path);

bool isRegularFile(const std::string& path);

std::string getDirectory(const std::string& path);

void  WriteFile(const std::string& iidfile, const std::string& imageID);

// int64_t Copy_directory(const fs::path& source, const fs::path& destination);
// void Copyfile(const fs::path& source, const fs::path& target, int64_t& total_size);
int64_t Copy_directory(const fs::path& sourcedir, const fs::path& destination);
bool IsPathSeparator(char c);
string join(const vector<string>& elem);
string Clean(const string& path);
bool isDirectoryEmpty(const boost::filesystem::path& dirPath);
std::string MkdirTemp(std::string dir, std::string pattern);
void Copy_file(const fs::path& src, const fs::path& dest);
bool isPathabsolute(const std::string& path);
#endif // GO_FILE_H