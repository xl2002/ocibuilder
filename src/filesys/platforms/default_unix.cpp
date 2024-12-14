#include "filesys/platforms/default_unix.h"
std::string GetWindowsOsVersion(){
    return "";
}

std::string cpuVariant() {
    #if defined(__arm__)
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