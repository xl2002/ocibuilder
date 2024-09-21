#if !defined(LOGRUS_BUFFERPOOL_H)
#define LOGRUS_BUFFERPOOL_H
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <condition_variable>
#include "bytes/buffer.h"

// 定义 BufferPool 接口
class BufferPool {
public:
    virtual ~BufferPool() = default;

    virtual void Put(std::shared_ptr<Buffer> buffer) = 0;
    virtual std::shared_ptr<Buffer> Get() = 0;
};


// 实现 BufferPool 的具体类 SimpleBufferPool
class SimpleBufferPool : public BufferPool {
public:
    std::queue<std::shared_ptr<Buffer>> pool; // 存储 Buffer 的队列
    std::mutex mtx; // 用于线程安全
    std::condition_variable cv; // 用于控制并发访问

public:
    // 使用 noexcept 声明析构函数
    ~SimpleBufferPool() noexcept override = default;

    // 回收 Buffer 到池中
    void Put(std::shared_ptr<Buffer> buffer) override {
        std::lock_guard<std::mutex> lock(mtx);
        pool.push(std::move(buffer));
        cv.notify_one(); // 通知等待的线程
    }

    // 获取一个 Buffer，如果池为空则创建新的
    std::shared_ptr<Buffer> Get() override {
        std::unique_lock<std::mutex> lock(mtx);
        if (pool.empty()) {
            return std::make_shared<Buffer>(); // 创建新的 Buffer
        }
        auto buffer = std::move(pool.front());
        pool.pop();
        return buffer;
    }
};
extern std::shared_ptr<BufferPool> globalBufferPool;

#endif // LOGRUS_BUFFERPOOL_H