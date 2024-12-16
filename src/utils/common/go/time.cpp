#include "utils/common/go/time.h"

std::string time_point_to_string(const std::chrono::system_clock::time_point& tp) {
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;

    // 根据不同的平台使用不同的线程安全 localtime 函数
#ifndef __linux__
    localtime_s(&tm, &time); // Windows 线程安全版本
#else
    localtime_r(&time, &tm); // POSIX 线程安全版本
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}