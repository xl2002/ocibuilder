#if !defined(PLATFORMS_CPUINFO_H)
#define PLATFORMS_CPUINFO_H
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "cobra/error.h"
#include "platforms/database.h"
std::string getCPUInfo(const std::string& pattern);
std::string getMachineArch();
std::string getCPUVariantFromArch(const std::string& arch);
std::string getCPUVariant();
std::string cpuVariant();

#endif