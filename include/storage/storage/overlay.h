#if !defined(STORAGE_STORAGE_OVERLAY_H)
#define STORAGE_STORAGE_OVERLAY_H

#include "filesys/graphdriver/driver.h"
#include "utils/common/error.h"
#include "filesys/graphdriver/driver_linux_windows.h"
#include "filesys/utils/idtools.h"
#include "filesys/graphdriver/chown.h"
#include "filesys/graphdriver/fsdiff.h"
#include <boost/algorithm/string.hpp>
class OverlayOptions;
std::shared_ptr<OverlayOptions> parseOptions(const std::vector<std::string>& options);
Driver Init(const std::string& home, const driver_Options& options);
std::string LookPath(const std::string& file);
extern std::string mountProgramFlagFile;
#endif