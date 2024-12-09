#include "image/buildah/retry.h"

void RetryIfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){
    IfNecessary(operation,options);
}
/**
 * @brief 如果需要，请使用重试选项以指数退避方式重试操作。
 * 
 * @param operation 
 * @param options 
 */
void IfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options){

}