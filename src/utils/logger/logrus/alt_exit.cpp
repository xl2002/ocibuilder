#include "utils/logger/logrus/alt_exit.h"


// 用于运行单个处理程序并捕获异常
void runHandler(const std::function<void()>& handler) {
    try {
        // 执行 handler
        handler();
    } catch (const myerror& e) {
        throw myerror("Error: Logrus exit handler error:");
    }
        
}

// 遍历并执行所有注册的 handlers
void runHandlers() {
    for (const auto& handler : handlers) {
        runHandler(handler);
    }
}
