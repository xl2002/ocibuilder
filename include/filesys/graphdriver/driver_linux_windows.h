#if !defined(GRAPHDRIVER_DRIVER_LINUX_WINDOWS_H) 
#define GRAPHDRIVER_DRIVER_LINUX_WINDOWS_H

#include <boost/filesystem.hpp>
#include <stdexcept>
#include <string>
#include <iostream>
#include "cobra/error.h"
#ifdef _WIN32
#include <windows.h>  // Windows 特定的头文件
#else
#include <sys/statvfs.h>  // 类 Unix 系统的头文件
#endif

// FsMagic 代表文件系统魔法常量
using FsMagic = uint32_t;
const FsMagic FsMagicUnsupported = 0xFFFFFFFF;  // 定义不支持的特殊值
const FsMagic FsMagicOverlay = FsMagic(0x794C7630);
const FsMagic FsMagicFUSE = FsMagic(0x65735546);
extern std::map<FsMagic, std::string> FsNames;
FsMagic GetFSMagic(const std::string& rootpath);
#endif