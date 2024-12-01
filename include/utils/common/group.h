#if !defined(HASHICORP_MULTIERROR)
#define HASHICORP_MULTIERROR
#include <future>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>
#include <memory>
#include "utils/common/error.h"

class Group
{
public:
    std::mutex mutex_;
    std::vector<std::future<void>> futures_;
    std::shared_ptr<myerror> err_=std::make_shared<myerror>();

public:
    void Go(std::function<void()> f);
    std::shared_ptr<myerror> Wait();
};

// 自定义的 WaitGroup 类
class WaitGroup {
public:
    WaitGroup() : count(0) {}

    void Add(int delta) {
        std::unique_lock<std::mutex> lock(mutex);
        if (count + delta < 0) {
            throw myerror("negative WaitGroup count");
        }
        count += delta;
    }

    void Done() {
        Add(-1);
    }

    void Wait() {
        std::unique_lock<std::mutex> lock(mutex);
        cond_var.wait(lock, [this] { return count == 0; });
    }

public:
    int count=0;
    std::mutex mutex;
    std::condition_variable cond_var;
};
#endif // HASHICORP_MULTIERROR