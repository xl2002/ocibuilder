#if !defined(FILESYS_GRAPHDRIVER_COUNTER_H)
#define FILESYS_GRAPHDRIVER_COUNTER_H

#include <map>
#include <mutex>
#include <string>
#include "filesys/graphdriver/driver_linux_windows.h"
class Checker_interface {
public:
    virtual bool IsMounted(const std::string& path) = 0;
};
// fsChecker 结构体实现
class FsChecker : public Checker_interface {
public:
    ~FsChecker() = default;
    FsChecker(FsMagic t) : t_(t) {}

    bool IsMounted(const std::string& path) override {
        bool mounted;
        // 假设 Mounted 是一个可以检查挂载状态的函数，返回值是 std::pair<bool, error> 类型
        // std::tie(mounted, std::ignore) = Mounted(t_, path); 
        return mounted;
    }

private:
    FsMagic t_;  // 保存 FsMagic 值
};

// NewFsChecker 函数实现
std::shared_ptr<Checker_interface> NewFsChecker(FsMagic t);
struct Minfo {
    bool check;
    int count;
};

class RefCounter {
public:
    std::map<std::string, Minfo> counts; // 假设 Minfo 是已定义的结构体
    std::mutex mu;
    std::shared_ptr<Checker_interface> checker=nullptr; // 修改为 shared_ptr 类型

    // 默认构造函数
    RefCounter() : checker(nullptr) {}

    // 构造函数，接受 shared_ptr 参数
    RefCounter(std::shared_ptr<Checker_interface> checker) : checker(checker) {
        // 其他初始化代码（如有需要）
    }
};

#endif