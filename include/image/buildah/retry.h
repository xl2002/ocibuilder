#if !defined(IMAGE_BUILDAH_RETRY_H)
#define IMAGE_BUILDAH_RETRY_H
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <utils/common/error.h>

namespace Retry{
    struct Options{
        int MaxRetry=0;  // 重试次数
        std::chrono::duration<int> Delay;  // 重试之间的延迟，使用chrono表示
        // std::function<bool(const std::exception&)> IsErrorRetryable;  // 错误是否可重试的函数
        Options()=default;
        Options(int maxRetry, std::chrono::duration<int> delay)
            : MaxRetry(maxRetry), Delay(delay) {}
    };
}
using RetryOptions=Retry::Options;
void RetryIfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options);
void IfNecessary(std::function<void()> operation, std::shared_ptr<Retry::Options> options);

#endif // IMAGE_BUILDAH_RETRY_H
