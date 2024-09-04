#if !defined(UTIL_UTIL_H)
#define UTIL_UTIL_H
#include <string>
#include "cobra/error.h"
#include "types/types.h"
#include <fstream>
#include "alltransports/alltransports.h"
#include "v1/descriptor.h"
const int minimumTruncatedIDLength=3;
const std::string DefaultTransport="docker://";


bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile);

std::string DiscoverContainerfile(std::string path);

std::string resolveSymlinks(const std::string& path);

std::shared_ptr<ImageReference> VerifyTagName(std::string imagespec);

Platform NormalizePlatform(Platform platform);
#endif // UTIL_UTIL_H
