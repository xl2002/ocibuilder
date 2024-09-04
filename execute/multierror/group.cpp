#include "multierror/group.h"
// #include <mutex>
/**
 * @brief 开启新线程并执行给定的函数
 * 
 * @param f 要执行的函数，函数的类型为std::function<std::string()>，返回值为std::string
 * 
 * 开启新线程并执行给定的函数，函数的返回值作为错误信息添加到错误列表中。
 * 如果函数的返回值不为空，则将错误信息添加到错误列表中。
 */
void Group::Go(std::function<void()> f) {
    // 获取互斥锁，确保在多线程访问时安全
    std::lock_guard<std::mutex> lock(mutex_);

    // 创建新线程并执行给定的函数,async启动异步任务
    futures_.emplace_back(std::async(std::launch::async, [this, f]() {
        // 执行给定的函数并获取返回值
        try {
            f();
        } catch (const myerror& e) {
            // 获取互斥锁，确保在多线程访问时安全
            std::lock_guard<std::mutex> lock(mutex_);
            // 将错误信息添加到错误列表中
            err_->AddError(e.what());
        }
        // // 如果返回值不为空，则将错误信息添加到错误列表中
        // if (!err.empty()) {
        //     // 获取互斥锁，确保在多线程访问时安全
        //     std::lock_guard<std::mutex> lock(mutex_);
        //     // 将错误信息添加到错误列表中
        //     err_->AddError(err);
        // }
    }));
}

/**
 * @brief 等待所有线程执行完成并返回收集到的错误列表
 * 
 * @return 返回包含所有错误信息的错误列表
 * 
 * 这个函数会等待所有线程执行完成，并返回一个包含所有错误信息的错误列表。
 * 函数会遍历所有开启的线程，并使用future对象的wait()方法等待线程执行完成。
 * 等待完成后，函数会返回一个包含所有错误信息的错误列表。
 */
std::shared_ptr<myerror> Group::Wait() {
    // 等待所有线程执行完成
    for (auto& future : futures_) {
        future.wait(); // 等待线程执行完成
    }
    // 返回包含所有错误信息的错误列表
    return err_;
}
