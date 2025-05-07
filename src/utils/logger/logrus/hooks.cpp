/**
 * @file hooks.cpp
 * @brief 实现日志钩子管理功能
 * @details 提供 LevelHooks 类的实现，用于管理和触发日志钩子
 */

#include <iostream>
#include "utils/logger/logrus/hooks.h"

// int Fire(LevelHooks &hooks, const Level &level, std::shared_ptr<Entry> entry)
// {
//     auto it = hooks.find(level);
//     if (it != hooks.end()) {
//         for (auto& hook : it->second) {
//             int err = hook.Fire(entry);
//             if (err != 0) {  // 假设 0 表示没有错误
//                 return err;
//             }
//         }
//     }
//     return 0;
// }
/**
 * @brief 添加钩子到 LevelHooks
 * @param hook 要添加的钩子接口指针
 * @details 将钩子添加到其支持的所有日志级别对应的钩子列表中
 */
void LevelHooks::Add(const std::shared_ptr<Hook_interface>& hook) {
    for (const auto& level : hook->Levels()) {
        hooks[level].push_back(hook);
    }
}

/**
 * @brief 为指定日志级别触发所有钩子
 * @param level 要触发钩子的日志级别
 * @param entry 日志条目
 * @throws myerror 当钩子执行抛出异常时
 * @details 触发指定级别下的所有钩子，如果任何钩子抛出异常，则包装为 myerror 抛出
 */
void LevelHooks::Fire(Level level, std::shared_ptr<Entry> entry) {
    if (hooks.find(level) == hooks.end()) return;

    for (const auto& hook : hooks[level]) {
        try {
            hook->Fire(entry);
        } catch (const std::exception& e) {
            throw myerror("Hook_interface execution error: "+std::string(e.what()));
        }
    }
}
