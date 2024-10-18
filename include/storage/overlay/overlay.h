#if !defined(STORAGE_OVERLAY_OVERLAY_H)
#define STORAGE_OVERLAY_OVERLAY_H

#include "graphdriver/driver.h"
#include "cobra/error.h"
#include "graphdriver/driver_linux_windows.h"
#include "idtools/idtools.h"
#include "graphdriver/chown.h"
#include "graphdriver/fsdiff.h"
#include <boost/algorithm/string.hpp>
std::shared_ptr<OverlayOptions> parseOptions(const std::vector<std::string>& options);
Driver Init(const std::string& home, const driver_Options& options);
std::string LookPath(const std::string& file);
#endif