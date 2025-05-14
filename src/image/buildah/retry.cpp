#include "image/buildah/retry.h"
#include <cmath>  // 包含cmath头文件
#include <thread>
#include <chrono>
#include <iostream>
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 执行操作并在失败时进行重试
 * 
 * @param operation 要执行的操作函数
 * @param options 重试配置选项
 * 
 * @details 该函数封装了IfNecessary()，提供简单的重试接口。
 * 如果操作抛出异常，会根据options配置进行重试。
 * 
 * @note 重试次数和延迟时间由options参数控制
 * @see IfNecessary
 * @see Retry::Options
 */
void RetryIfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    try{
        IfNecessary(operation,options);
    }catch (const std::exception& e){
        throw;
    }
}
/**
 * @brief 实现带指数退避的重试机制
 * 
 * @param operation 要执行的操作函数
 * @param options 重试配置选项
 * 
 * @details 该函数实现了指数退避算法，在操作失败时，
 * 会按照options.MaxRetry指定的次数进行重试，
 * 每次重试间隔时间=options.Delay * 2^attempt
 * 
 * @note 重试过程中会输出重试信息到标准输出
 * @throw 如果所有重试都失败，会抛出最后一次的异常
 * @see Retry::Options
 */
void IfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    int attempt = 0;
    try{
        operation(); // 执行操作
    }catch(const std::exception& e){
        for(;attempt<options->MaxRetry;attempt++){
            double delayInSeconds = std::chrono::duration_cast<std::chrono::seconds>(options->Delay).count();
            logger->log_warning("failed,retryong in "+std::to_string(delayInSeconds*std::pow(2,attempt))+" seconds "+"("+std::to_string(attempt+1)+"/"+std::to_string(options->MaxRetry)+")");
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
