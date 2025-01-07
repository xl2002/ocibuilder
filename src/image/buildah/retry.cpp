#include "image/buildah/retry.h"
#include <cmath>  // 包含cmath头文件
#include <thread>
#include <chrono>
#include <iostream>
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
        for(;attempt<options->MaxRetry;attempt++){
            double delayInSeconds = std::chrono::duration_cast<std::chrono::seconds>(options->Delay).count();
            std::cout<<"failed,retryong in "<<delayInSeconds*std::pow(2,attempt)<<" seconds "<<"("<<attempt+1<<"/"<<options->MaxRetry<<")"<<std::endl;
            std::this_thread::sleep_for(options->Delay*std::pow(2,attempt));
            try{
                operation(); // 执行操作
            }catch(const std::exception& e){
                continue;
            }
        }
    }
    // operation(); // 执行操作
} 