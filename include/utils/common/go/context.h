#if !defined(GO_CONTEXT_H)
#define GO_CONTEXT_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <thread>

class Context {
public:
    // 默认构造函数，创建未取消的Context
    Context() : canceled(false) {}

    // 设置超时的构造函数，自动取消Context
    explicit Context(std::chrono::milliseconds timeout) : canceled(false) {
        // 创建一个线程，用于处理超时取消
        cancel_thread = std::thread([this, timeout]() {
            std::this_thread::sleep_for(timeout);
            cancel();
        });
    }

    // 析构函数，确保取消线程被正确回收
    ~Context() {
        cancel(); // 确保取消信号被触发
        if (cancel_thread.joinable()) {
            cancel_thread.join(); // 等待取消线程结束
        }
    }

    // 返回是否取消
    bool Done() const {
        return canceled.load();
    }

    // 获取取消时的错误
    std::runtime_error Err() const {
        if (canceled.load()) {
            return std::runtime_error("context canceled");
        }
        return std::runtime_error("context not canceled");
    }

    // 主动取消 Context
    void cancel() {
        std::lock_guard<std::mutex> lock(mu);
        canceled.store(true);
        cv.notify_all();
    }

    // 等待Context完成或超时
    bool WaitForCompletion(std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(mu);
        return cv.wait_for(lock, timeout, [this]() { return canceled.load(); });
    }

public:
    std::atomic<bool> canceled;                // 表示是否已经取消
    std::mutex mu;                             // 互斥锁，用于保护取消状态
    std::condition_variable cv;                // 条件变量，用于通知等待的线程
    std::thread cancel_thread;                 // 用于处理超时取消的线程
};

#endif // GO_CONTEXT_H)
