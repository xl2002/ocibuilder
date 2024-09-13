#if !defined(PLATFORMS_DEFAULT_UNIX_H)
#define PLATFORMS_DEFAULT_UNIX_H

#include <string>
#include <vector>
#include <memory>
#include "v1/descriptor.h"
#ifdef _WIN32
    #include <windows.h>
    const std::string GOOS = "windows";
#else
    #include <unistd.h>
    const std::string GOOS = "linux"; // 假设默认是 Linux，可以根据实际系统修改
#endif

#if defined(__x86_64__) || defined(_M_X64)
    const std::string GOARCH = "amd64";
#elif defined(__i386__) || defined(_M_IX86)
    const std::string GOARCH = "386";
#elif defined(__arm__) || defined(_M_ARM)
    const std::string GOARCH = "arm";
#elif defined(__aarch64__)
    const std::string GOARCH = "arm64";
#else
    const std::string GOARCH = "unknown";
#endif

// 返回当前CPU的变体，如果不是ARM，则为空
std::string cpuVariant() {
#ifdef __arm__
    // ARM 平台下可以设置具体的变体信息
    return "v7";  // 示例值，可根据需要调整
#else
    return "";
#endif

}

std::shared_ptr<Platform> DefaultSpec(){
    auto platform = std::make_shared<Platform>();
    platform->Architecture = GOARCH;
    platform->OS = GOOS;
    platform->Variant = cpuVariant();
    return platform;
}
#endif // PLATFORMS_DEFAULT_UNIX_H)
