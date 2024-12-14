#if !defined(FILESYS_SYSTEM_RUSAGE_RUSAGE_UNIX_H)
#define FILESYS_SYSTEM_RUSAGE_RUSAGE_UNIX_H
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <utility> // For std::pair
#include "filesys/system/rusage/rusage.h"
#include "filesys/systems.h"

// 将 timeval 结构转换为 std::chrono::duration
std::chrono::duration<double> mkDuration(const struct timeval& tv) {
    return std::chrono::seconds(tv.tv_sec) + std::chrono::microseconds(tv.tv_usec);
}

// 获取资源使用情况
std::pair<Rusage, std::exception_ptr> get() {
    Rusage r;
    r.Date = std::chrono::system_clock::now();

#ifdef __linux__
    // Linux 平台的实现
    struct rusage usage;
    if (getrusage(RUSAGE_CHILDREN, &usage) != 0) {
        return {r, std::current_exception()};
    }

    r.Utime = mkDuration(usage.ru_utime);
    r.Stime = mkDuration(usage.ru_stime);
    r.Inblock = static_cast<int64_t>(usage.ru_inblock);
    r.Outblock = static_cast<int64_t>(usage.ru_oublock);

    return {r, nullptr};
#else
return {r, nullptr};
#endif
}

// 检查资源使用计数器是否被支持
bool Supported() {
#ifdef __linux__
    return true;
#else
    // Windows 平台的实现
    // 这里假设 Windows 总是支持资源计数器
    return true;
#endif
}
// 错误处理函数，将 std::exception_ptr 转换为字符串
std::string errorToString(std::exception_ptr eptr) {
    if (!eptr) return "";

    try {
        std::rethrow_exception(eptr);
    } catch (const std::exception& e) {
        return e.what();
    } catch (...) {
        return "Unknown error";
    }
}

std::pair<Rusage, std::exception_ptr> Get(){
    auto r=get();
    if(r.second){
        // throw myerror(errorToString(r.second));
        return {Rusage(), r.second};
    }
    return r;
}
#endif // RUSAGE_RUSAGE_UNIX_H)
