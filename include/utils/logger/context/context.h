#if !defined(CONTEXT_CONTEXT_H)
#define CONTEXT_CONTEXT_H
#include <chrono>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <any>
#include <iostream>
#include <boost/any.hpp>
#include <boost/optional.hpp>
class Context : public std::enable_shared_from_this<Context> {
    public:
    
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
    //Context() : done_(promise_.get_future().share()) {}

    boost::optional<TimePoint> deadline_;    // 截止时间
    std::shared_ptr<void> done_;        // 表示上下文何时完成的 future
    std::promise<void> promise_;           // 用于设置 done_ 的 promise
    boost::optional<std::string> err_;       // 错误信息
    std::map<std::string, boost::any> values_; // 用于存储键值对的 map
public:

    // Deadline 返回该上下文执行工作应被取消的时间。
    // 如果未设置截止时间，则返回 std::nullopt。
    boost::optional<TimePoint> Deadline() const {
        return deadline_;
    }

    // Done 返回一个 future，当该上下文相关的工作应被取消时，该 future 将会就绪。
    std::shared_ptr<void> Done() const {
        return done_;
    }

    // 如果 Done 尚未就绪，Err 返回 std::nullopt。
    // 如果 Done 就绪，Err 返回一个解释原因的错误字符串：
    // "Canceled" 如果上下文被取消，或者 "DeadlineExceeded" 如果上下文的截止时间已过。
    boost::optional<std::string> Err() const {
        // if (done_.valid() && done_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        //     return err_;
        // }
        return boost::none;
    }

    // Value 返回与该上下文关联的键值，若没有与键关联的值，则返回 std::nullopt。
    boost::optional<boost::any> Value(const std::string& key) const {
        auto it = values_.find(key);
        if (it != values_.end()) {
            return it->second;
        }
        return boost::none;
    }

    // 设置截止时间
    void SetDeadline(TimePoint deadline) {
        deadline_ = deadline;
    }

    // 取消上下文
    void Cancel() {
        err_ = "Canceled";
        promise_.set_value();
    }

    // 设置上下文中的键值对
    void SetValue(const std::string& key, boost::any value) {
        values_[key] = value;
    }

    // 创建一个带有取消功能的上下文
    static std::shared_ptr<Context> WithCancel() {
        return std::make_shared<Context>();
    }


};

#endif // CONTEXT_CONTEXT_H