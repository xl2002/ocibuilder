#include "filesys/platforms/default_unix.h"
/**
 * @brief 获取Windows操作系统版本(在Unix平台返回空字符串)
 * @return 空字符串(Unix平台实现)
 */
std::string GetWindowsOsVersion(){
    return "";
}

/**
 * @brief 获取CPU变体信息
 * @return CPU变体字符串
 * @note 在ARM平台返回"v7"，其他平台返回空字符串
 */
std::string cpuVariant() {
    #if defined(__arm__)
        // ARM 平台下可以设置具体的变体信息
        return "v7";  // 示例值，可根据需要调整
    #else
        return "";
    #endif
}

/**
 * @brief 创建默认平台规范对象
 * @return 包含默认架构、操作系统和变体的Platform对象指针
 * @note 使用GOARCH和GOOS宏定义默认值
 */
std::shared_ptr<Platform> DefaultSpec(){
    auto platform = std::make_shared<Platform>();
    platform->Architecture = GOARCH;
    platform->OS = GOOS;
    platform->Variant = cpuVariant();
    return platform;
}
