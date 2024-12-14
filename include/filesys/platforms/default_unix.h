#if !defined(FILESYS_PLATFORMS_DEFAULT_UNIX_H)
#define FILESYS_PLATFORMS_DEFAULT_UNIX_H

#include <string>
#include <vector>
#include <memory>
#include "image/image_types/v1/descriptor.h"
#include "filesys/systems.h"


// #if defined(__x86_64__) || defined(_M_X64)
//     const std::string GOARCH = "amd64";
// #elif defined(__i386__) || defined(_M_IX86)
//     const std::string GOARCH = "386";
// #elif defined(__arm__) || defined(_M_ARM)
//     const std::string GOARCH = "arm";
// #elif defined(__aarch64__)
//     const std::string GOARCH = "arm64";
// #else
//     const std::string GOARCH = "unknown";
// #endif

// 返回当前CPU的变体，如果不是ARM，则为空
extern std::string GOOS;
extern std::string GOARCH;
std::string GetWindowsOsVersion();
std::string cpuVariant();
std::shared_ptr<Platform> DefaultSpec();


#endif // PLATFORMS_DEFAULT_UNIX_H)
