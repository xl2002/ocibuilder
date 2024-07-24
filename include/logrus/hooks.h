#if !defined(LOGRUS_HOOKS_H)
#define LOGRUS_HOOKS_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include "logrus/logrus.h"
//定义 Level 枚举类型
//using Level = uint32_t;
// enum class Level : uint32_t {
//     Debug = 0,
//     Info,
//     Warn,
//     Error,
//     Fatal
// };
// // 定义 LevelHooks 接口
// using Hook = std::function<void()>;
// using LevelHooks = std::unordered_map<Level, std::vector<Hook>>;
class Hook{
public:
    virtual std::vector<Level> Levels() = 0;
    virtual void Fire()=0;
    virtual ~Hook() = default;

};
// // 定义 AddHook
// void AddHook(Level level, const std::function<void()>& hook) {
//     levelHooks[level].push_back(hook);
// }

#endif // LOGRUS_HOOKS_H

