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
// 添加钩子到 LevelHooks
void LevelHooks::Add(const std::shared_ptr<Hook_interface>& hook) {
    for (const auto& level : hook->Levels()) {
        hooks[level].push_back(hook);
    }
}

// 为传递的级别触发所有钩子
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