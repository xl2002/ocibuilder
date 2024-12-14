#include "image/buildah/retry.h"
#include <cmath>  // 包含cmath头文件
#include <thread>
#include <chrono>

void RetryIfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    IfNecessary(operation,options);
}
/**
 * @brief 如果需要，请使用重试选项以指数退避方式重试操作。
 * 
 * @param operation 
 * @param options 
 */
std::error_code IfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    int attempt = 0;
        std::error_code lastErr;
        while (attempt < options->MaxRetry) {
            try {
                operation(); // 执行操作
                return; // 如果成功，直接返回
            } catch (const std::exception& e) {
                lastErr = std::make_error_code(std::errc::io_error); // 捕获异常
            }

            // // 如果有指定的可重试错误函数，则使用该函数判断是否继续重试
            // if (options->isErrorRetryable && !options->isErrorRetryable(lastErr)) {
            //     break; // 如果错误不可重试，退出
            // }
            // // 指数退避：退避延迟增加

            // std::this_thread::sleep_for(options->Delay * static_cast<int>(std::pow(2, attempt)));

            
            ++attempt;
        }

        // 超过最大重试次数仍未成功，抛出异常
        if (attempt == options->MaxRetry) {
            throw std::runtime_error("Operation failed after retries.");
        }
        return lastErr;
    
} 