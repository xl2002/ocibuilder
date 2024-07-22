package multierror

import "sync"

// Group is a collection of goroutines which return errors that need to be
// coalesced.
class Group {
public:
    std::mutex mutex;
    std::shared_ptr<Error> err;
    std::condition_variable cv;
    std::atomic<int> wg;
};

void Group::Go(std::function<std::string()> f) {
    std::lock_guard<std::mutex> lock(mutex_);
    futures_.emplace_back(std::async(std::launch::async, [this, f]() {
        std::string err = f();
        if (!err.empty()) {
            std::lock_guard<std::mutex> lock(mutex_);
            err_->AddError(err);
        }
    }));
    ++wg_;
}

// Wait方法阻塞，直到Go方法的所有函数调用返回，然后返回multierror。
std::shared_ptr<myerror> Group::Wait() {
    for (auto& future : futures_) {
        future.wait();
    }
    return err_;
}
