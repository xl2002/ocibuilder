#if !defined(CONFIG_DEFAULT_LINUX_H)
#define CONFIG_DEFAULT_LINUX_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>         // 用于字符串转换
#include "config/config.h"
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
using rlim_t = size_t; // 在Windows中使用size_t作为rlim_t的替代
#else
#include <sys/resource.h>
//#include <unistd.h>
#endif
std::string getDefaultLockType();
std::string getLibpodTmpDir();
std::vector<std::string> getDefaultMachineVolumes();
std::vector<std::string> getDefaultProcessLimits();
bool selinuxEnabled();

extern std::string OverrideContainersConfig;
extern std::string DefaultContainersConfig;
extern std::string DefaultSignaturePolicyPath;
extern rlim_t oldMaxSize;
#endif // CONFIG_DEFAULT_LINUX_H
