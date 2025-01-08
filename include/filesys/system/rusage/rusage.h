#if !defined(FILESYS_SYSTEM_RUSAGE_RUSAGE_H)
#define FILESYS_SYSTEM_RUSAGE_RUSAGE_H
#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
// #include <chrono>
// #include "filesys/system/rusage/rusage_unix.h"
#include "utils/common/error.h" 
// Rusage 结构体表示当前进程及其子进程的Unix风格资源使用计数器子集。
// 在某些系统调用不可用的平台上（例如，不存在getrusage()的系统），计数器总是为0。
struct Rusage {
    std::chrono::system_clock::time_point Date;   // 记录当前时间
    std::chrono::duration<double> Elapsed;        // 记录经过的时间
    std::chrono::duration<double> Utime;          // 用户态CPU时间
    std::chrono::duration<double> Stime;          // 内核态CPU时间
    int64_t Inblock = 0;                          // 输入块计数
    int64_t Outblock = 0;                         // 输出块计数

    // 构造函数
    Rusage() = default;
    // Subtract 函数，用于计算差异
    Rusage Subtract(const Rusage& baseline) const {
        Rusage result;
        result.Elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(Date - baseline.Date);  // 计算经过时间
        result.Utime = Utime - baseline.Utime;     // 计算用户态CPU时间的差值
        result.Stime = Stime - baseline.Stime;     // 计算内核态CPU时间的差值
        result.Inblock = Inblock - baseline.Inblock; // 计算输入块计数差值
        result.Outblock = Outblock - baseline.Outblock; // 计算输出块计数差值
        return result;
    }
};
// std::pair<Rusage, std::exception_ptr> get();
// 将时间格式化为字符串
std::string formatDuration(const std::chrono::duration<double>& duration) {
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    std::stringstream ss;
    ss << millis / 1000 << '.' << std::setfill('0') << std::setw(3) << millis % 1000;
    return ss.str() + "s";
}

// 将字节大小转换为人类可读格式
std::string humanSize(double size) {
    const char* suffix[] = { "B", "KB", "MB", "GB", "TB" };
    int i = 0;
    while (size >= 1024 && i < 4) {
        size /= 1024;
        ++i;
    }
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4) << size << suffix[i];
    return ss.str();
}
// 格式化 Rusage 的差异
std::string FormatDiff(const Rusage& diff) {
    std::stringstream ss;
    ss << formatDuration(diff.Stime) << "(system) "
        << formatDuration(diff.Utime) << "(user) "
        << formatDuration(diff.Elapsed) << "(elapsed) "
        << humanSize(static_cast<double>(diff.Inblock * 512)) << " input "
        << humanSize(static_cast<double>(diff.Outblock * 512)) << " output";
    return ss.str();
}
#endif // RUSAGE_RUSAGE_H)
