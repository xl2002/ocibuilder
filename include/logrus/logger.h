#if !defined(LOGRUS_LOGGER_H)
#define LOGRUS_LOGGER_H
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <atomic>
#include <fstream>
#include "logrus/hooks.h"
#include "logrus/formatter.h"
#include "sync/pool.h"
#include "logrus/bufferpool.h"
#include "sync/mutex.h"
// using namespace std;
// 定义 MutexWrap
class MutexWrap {
public:
    Mutex lock;
    bool disabled = false;

    void Lock() {
        mtex.lock();
    }

    void Unlock() {
        mtex.unlock();
    }

private:
    std::mutex mtex;
    //
};
// 定义 exitFunc
using exitFunc = std::function<void(int)>;
//exitFunc ExitFunc = [](int code) { std::exit(code); };

// 定义 make_unique 辅助函数
// template <typename T, typename... Args>
// std::unique_ptr<T> make_unique(Args&&... args) {
//     return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
// }

// class Logger {
// public:
//     // 日志在互斥锁中通过 `io.Copy` 写入此处。通常将其设置为文件，
//     // 或保留默认值 `os.Stderr`。你也可以将其设置为更具冒险性的目标，
//     // 比如记录到 Kafka。
//     std::shared_ptr<std::ofstream> Out;
//     // 日志实例的钩子。这些钩子允许基于日志级别和日志条目触发事件。
//     // 例如，可以将错误发送到错误跟踪服务、记录到 StatsD，
//     // 或在致命错误时转储核心。
//     LevelHooks Hooks;
//     // 所有日志条目在记录到 Out 之前都会经过格式化器。
//     // 内置的格式化器有 `TextFormatter` 和 `JSONFormatter`，其中 `TextFormatter` 是默认格式化器。
//     // 在开发环境中（当附加了 TTY 时），它会使用颜色记录日志，但在记录到文件时不会。
//     // 你可以轻松实现自己的 `Formatter` 接口，示例请参见 `README` 或内置格式化器。
//     std::shared_ptr<Formatter> Formatter;

//     // 是否记录调用者信息的标志（默认关闭）
//     bool ReportCaller;

//     // 日志记录器应记录的日志级别。通常（也是默认）为 `logrus.Info`，
//     // 这允许记录 Info()、Warn()、Error() 和 Fatal()。
//     //Level Level;
//     std::atomic<Level> Level{Level::Info};
//     // 用于同步写入日志。默认启用锁定
//     MutexWrap mu;
//     // 可重复使用的空条目
//     Pool entryPool;
//     // 退出应用程序的函数，默认值为 `os.Exit()`
//     exitFunc ExitFunc;
//     // 用于格式化日志的缓冲池。如果为 nil，将使用默认的全局缓冲池。
//     std::shared_ptr<BufferPool> BufferPool;

//     Logger() = default;
    
//     static std::unique_ptr<Logger> New() {
//         return make_unique<Logger>();
//     }

//     // SetOutput 设置日志输出
//     void SetOutput(std::ostream* output) {
//         mu.Lock();
//         Out = output;
//         mu.Unlock();
//     }
//     //设置日志级别
//     void SetLevel(Level level){
//         Level.store(level, std::memory_order_relaxed);
//     }
//     // Constructor to initialize the logger with a default level
//     //Logger(Logger::Level level) : level_(level) {}

//     // 获取当前日志级别
//     Level GetLevel() const {
//         return Level.load(std::memory_order_relaxed);
//     }

//     //Debugf
// };



Level SetLelvel();

#endif // LOGRUS_LOGGER_H