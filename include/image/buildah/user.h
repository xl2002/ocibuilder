#pragma once

#ifndef TAR_USER_H
#define TAR_USER_H

#include <string>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <windows.h>  // 用于获取Windows用户信息
#include <cstdint>    // 用于定义 uint64_t
typedef uint64_t uid_t;  // 为 Windows 定义与 tar 兼容的 uid_t 类型
typedef uint64_t gid_t;  // 为 Windows 定义与 tar 兼容的 gid_t 类型
#else
#include <pwd.h>      // POSIX系统的用户信息
#include <grp.h>      // POSIX系统的组信息
#endif

namespace tarpp {
namespace user {

// Helper function to format the UID/GID as octal (as required by tar)
inline std::string to_octal_string(uint64_t value)
{
    std::stringstream ss;
    ss << std::oct << value;  // Convert to octal string
    return ss.str();
}

// 获取用户名称
inline std::string get_user_name(uid_t uid)
{
#ifdef _WIN32
    // Windows平台：直接返回固定值（你可以改为其他固定值或通过 Windows API 获取实际用户名）
    return "000000";  // 模拟用户 ID 为 "000000"
#else
    // POSIX平台：使用getpwuid_r获取用户名
    static constexpr size_t BUFFER_SIZE = 16384;
    passwd pwd{};
    passwd* result;
    char buffer[BUFFER_SIZE];

    getpwuid_r(uid, &pwd, buffer, BUFFER_SIZE, &result);
    if (result == nullptr) {
        return "ERROR";
    }

    return pwd.pw_name;
#endif
}

// 获取组名称
inline std::string get_group_name(gid_t gid)
{
#ifdef _WIN32
    // Windows平台：直接返回固定值（你可以改为其他固定值或通过 Windows API 获取实际组名）
    return "000000";  // 模拟组 ID 为 "000000"
#else
    // POSIX平台：使用getgrgid_r获取组名
    static constexpr size_t BUFFER_SIZE = 16384;
    group grp{};
    group* result;
    char buffer[BUFFER_SIZE];

    getgrgid_r(gid, &grp, buffer, BUFFER_SIZE, &result);
    if (result == nullptr) {
        return "ERROR";
    }

    return grp.gr_name;
#endif
}

// 将 `uid_t` 和 `gid_t` 转换为 tar 头部的格式（八进制形式）
inline std::string get_octal_uid(uid_t uid)
{
    return to_octal_string(uid);
}

inline std::string get_octal_gid(gid_t gid)
{
    return to_octal_string(gid);
}

// Windows下的模拟getuid：返回固定的用户ID（0）
inline uid_t getuid()
{
    return 0;  // 返回固定值0，模拟用户ID
}

// Windows下的模拟getgid：返回固定的组ID（0）
inline gid_t getgid()
{
    return 0;  // 返回固定值0，模拟组ID
}

}} // tarpp::user

#endif //TAR_USER_H
