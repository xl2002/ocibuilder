#include "semaphore/semaphore.h"

std::shared_ptr<Weighted> NewWeighted(int64_t n) {
    return std::make_shared<Weighted>(n);
}
// Acquire 函数，用于获取信号量
bool  Weighted::Acquire( int64_t n) {
    std::unique_lock<std::mutex> lock(mu);  // 互斥锁

    // 如果有足够的资源并且没有等待者，立即获取资源
    if (size - cur >= n && waiters.empty()) {
        cur += n;
        lock.unlock();
        return true; // 返回成功
    }

    // 如果请求的资源超过了最大容量，立即返回错误
    if (n > size) {
        lock.unlock();
        // 等待上下文完成
        // while (!ctx.Done()) {
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 模拟阻塞等待
        // }
        // return ctx.Err(); // 返回上下文的错误
        return false;
    }

    // 创建等待者
    waiter w{n, std::promise<std::string>{}};
    std::future<std::string> fut = w.ready.get_future(); // 获取等待的 future
    waiters.push_back(std::move(w));
    auto elem = --waiters.end();  // 获取队列中的迭代器位置
    lock.unlock();

    // 等待资源或者上下文完成
    std::future_status status = fut.wait_for(std::chrono::milliseconds(1));
    // if (ctx.Done()) {
    if(true){
        // std::error_code err = ctx.Err();
        lock.lock();
        if (fut.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            // 已经获取了信号量，忽略取消
            // err = std::error_code();
        } else {
            bool isFront = (waiters.begin() == elem);
            waiters.erase(elem);
            // 如果当前元素在队列最前面，且还有多余的资源，通知其他等待者
            if (isFront && size > cur) {
                notifyWaiters();
            }
        }
        lock.unlock();
        return false;
    }

    // 成功获取资源
    // return std::error_code();
    return true;
    
}

// 通知等待者的方法
void Weighted::notifyWaiters() {
    std::lock_guard<std::mutex> lock(mu); // 确保互斥锁的自动释放

    while (true) {
        if (waiters.empty()) {
            break; // 没有更多的等待者阻塞
        }

        // 获取队列的第一个等待者
        auto next = std::move(waiters.front());
        if (size - cur < next.n) {
            // 对于下一个等待者，没有足够的资源
            // 不继续遍历，以避免大请求的饥饿问题
            break;
        }

        // 更新当前资源使用量
        cur += next.n;

        // 移除已经被处理的等待者
        waiters.pop_front();

        // 通知等待者其请求已被满足
        next.ready.set_value("");
    }
}

// 释放信号量的方法，传入释放的权重 n
void Weighted::Release(int64_t n) {
    std::unique_lock<std::mutex> lock(mu); // 自动加锁和解锁

    // 释放资源
    cur -= n;

    // 检查是否释放的资源超过持有的资源量
    if (cur < 0) {
        lock.unlock();
        throw std::runtime_error("semaphore: released more than held");
    }

    // 通知等待者
    notifyWaiters();
    lock.unlock();
}
// 尝试获取信号量，传入请求的权重 n，如果成功则返回 true，失败则返回 false
bool Weighted::TryAcquire(int64_t n) {
    std::lock_guard<std::mutex> lock(mu); // 自动加锁和解锁

    // 判断是否可以获取资源
    bool success = (size - cur >= n) && waiters.empty();
    if (success) {
        cur += n; // 更新当前使用的资源量
    }
    return success; // 返回获取资源的结果
}