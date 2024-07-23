#if !defined(LOGRUS_HOOKS_H)
#define LOGRUS_HOOKS_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

//定义 Level 枚举类型
//using Level = uint32_t;
enum class Level : uint32_t {
    Debug = 0,
    Info,
    Warn,
    Error,
    Fatal
};
// 定义 LevelHooks 接口
using Hook = std::function<void()>;
using LevelHooks = std::unordered_map<Level, std::vector<Hook>>;

#endif // LOGRUS_HOOKS_H

