#if !defined(UTIL_UTIL_H)
#define UTIL_UTIL_H
#include <string>
#include "cobra/error.h"
#include "types/types.h"
#include <fstream>

bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile);

std::string DiscoverContainerfile(std::string path);

std::string resolveSymlinks(const std::string& path);
#endif // UTIL_UTIL_H
