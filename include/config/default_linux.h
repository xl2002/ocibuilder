#if !defined(CONFIG_DEFAULT_LINUX_H)
#define CONFIG_DEFAULT_LINUX_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>         // 用于字符串转换
#include "config/config.h"
std::string getDefaultLockType();
std::string getLibpodTmpDir();
std::vector<std::string> getDefaultMachineVolumes();
std::vector<std::string> getDefaultProcessLimits();
bool selinuxEnabled();

extern const std::string OverrideContainersConfig;
extern const std::string DefaultContainersConfig;
extern const std::string DefaultSignaturePolicyPath;

#endif // CONFIG_DEFAULT_LINUX_H
