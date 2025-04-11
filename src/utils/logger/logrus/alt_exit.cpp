#include "utils/logger/logrus/alt_exit.h"


/// @brief 运行单个处理程序并捕获异常
/// @param handler 要执行的处理程序函数
/// @throw myerror 当处理程序抛出异常时
void runHandler(const std::function<void()>& handler) {
    try {
        // 执行 handler
        handler();
    } catch (const myerror& e) {
        throw myerror("Error: Logrus exit handler error:");
    }
        
}

/// @brief 遍历并执行所有注册的处理程序
/// @details 按注册顺序依次调用每个处理程序
void runHandlers() {
    for (const auto& handler : handlers) {
        runHandler(handler);
    }
}
