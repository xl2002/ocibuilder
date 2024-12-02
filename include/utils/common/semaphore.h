#if !defined(UTILS_COMMON_SEMAPHORE_H)
#define UTILS_COMMON_SEMAPHORE_H
#include <string>
#include <vector>
#include <mutex>
#include <list>
#include <memory>
#include <condition_variable>
#include <future>
#include <stdexcept>
#include <chrono>
#include <utility>
// 等待者结构体，用于存储等待队列中的请求信息
struct waiter {
    int64_t n=0;                // 请求的资源数
    std::promise<std::string> ready; // 模拟Go中的chan，用于通知
    waiter()=default;
    waiter(int64_t n,std::promise<std::string>&& ready):n(n),ready(std::move(ready)){}
    // 允许移动构造函数和移动赋值运算符
    waiter(waiter&&) = default;
    waiter& operator=(waiter&&) = default;
};

class Weighted{
    public:
    int64_t size=0;
    int64_t cur=0;
    std::mutex mu;
    std::list<waiter> waiters;
    Weighted()=default;
    Weighted(int64_t n):size(n),cur(0){};
    bool Acquire(int64_t n);
    void notifyWaiters();
    void Release(int64_t n);
    bool TryAcquire(int64_t n);
};

std::shared_ptr<Weighted> NewWeighted(int64_t n);

#endif // SEM_SEMAPHORE_H
