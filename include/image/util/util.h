#if !defined(IMAGE_UTIL_UTIL_H)
#define IMAGE_UTIL_UTIL_H
#include <string>
#include <set>
#include "utils/common/error.h"
#include "image/types/types.h"
#include <fstream>
#include "image/transports/alltransports.h"
#include "image/image_types/v1/descriptor.h"
const int minimumTruncatedIDLength=3;
const std::string DefaultTransport="docker://";


bool MirrorToTempFileIfPathIsDescriptor(std::string file ,std::string &authfile);

std::string DiscoverContainerfile(std::string path);

std::string resolveSymlinks(const std::string& path);

std::shared_ptr<ImageReference_interface> VerifyTagName(std::string imagespec);

Platform NormalizePlatform(Platform platform);

bool SetHas(std::set<std::string> m,std::string k);
#endif // UTIL_UTIL_H