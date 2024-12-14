#include "image/buildah/retry.h"
#include <cmath>  // 包含cmath头文件
#include <thread>
#include <chrono>

void RetryIfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    try{
        IfNecessary(operation,options);
    }catch (const std::exception& e){
        throw;
    }
}
/**
 * @brief 如果需要，请使用重试选项以指数退避方式重试操作。
 * 
 * @param operation 
 * @param options 
 */
void IfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    int attempt = 0;
    try{
        operation(); // 执行操作
    }catch(const std::exception& e){
        throw;
    }
    // operation(); // 执行操作
} 