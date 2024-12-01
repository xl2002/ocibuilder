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
#include <any>
#include <cstdarg> // For va_list, va_start, va_end
#include "utils/logger/logrus/hooks.h"
#include "utils/logger/logrus/formatter.h"
#include "utils/logger/sync/pool.h"
#include "utils/logger/logrus/bufferpool.h"
#include "utils/logger/sync/mutex.h"
#include "utils/logger/logrus/text_formatter.h"
#include "utils/logger/logrus/logrus.h"
#include "utils/logger/logrus/alt_exit.h"
#include <boost/any.hpp>
#include "utils/logger/context/context.h"
// using namespace std;
// 定义 MutexWrap
class MutexWrap {
public:
    std::mutex lock;
    bool disabled = false;
    
public:
    void Lock() {
        if (!disabled) {
            lock.lock();
        }
    }

    void Unlock() {
        if (!disabled) {
            lock.unlock();
        }
    }

    void Disable() {
        disabled = true;
    }

};
// 定义 exitFunc 
using exitFunc = std::function<void(int)>;
//exitFunc ExitFunc = [](int code) { std::exit(code); };

// // 定义 make_shared 辅助函数
// template <typename T, typename... Args>
// std::shared_ptr<T> make_shared(Args&&... args) {
//     return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
// }


// 定义 LogFunction 类型  一个返回std::vecter<boost::any>的函数
using LogFunction = std::function<std::vector<boost::any>()>;
class LevelHooks;
class Level;
class Entry;
class Hook_interface;
class Logger : public std::enable_shared_from_this<Logger> {
public:
    // 日志在互斥锁中通过 `io.Copy` 写入此处。通常将其设置为文件，
    // 或保留默认值 `os.Stderr`。你也可以将其设置为更具冒险性的目标，
    // 比如记录到 Kafka。
    std::shared_ptr<std::ofstream> Out=std::make_shared<std::ofstream>();
    // 日志实例的钩子。这些钩子允许基于日志级别和日志条目触发事件。
    // 例如，可以将错误发送到错误跟踪服务、记录到 StatsD，
    // 或在致命错误时转储核心。
    std::shared_ptr<LevelHooks> Hooks=std::make_shared<LevelHooks>();
    // 所有日志条目在记录到 Out 之前都会经过格式化器。
    // 内置的格式化器有 `TextFormatter` 和 `JSONFormatter`，其中 `TextFormatter` 是默认格式化器。
    // 在开发环境中（当附加了 TTY 时），它会使用颜色记录日志，但在记录到文件时不会。
    // 你可以轻松实现自己的 `Formatter` 接口，示例请参见 `README` 或内置格式化器。
    std::shared_ptr<Formatter_interface> FormatterPtr=std::make_shared<TextFormatter>();

    // 是否记录调用者信息的标志（默认关闭）
    bool ReportCaller=false;

    // 日志记录器应记录的日志级别。通常（也是默认）为 `logrus.Info`，
    // 这允许记录 Info()、Warn()、Error() 和 Fatal()。
    std::shared_ptr<Level> currentLevel=std::make_shared<Level>();
    //std::atomic<Level> Level{Level::Info};
    // 用于同步写入日志。默认启用锁定
    std::shared_ptr<MutexWrap> mu=std::make_shared<MutexWrap>();
    // 可重复使用的空条目
    std::shared_ptr<Pool> entryPool=std::make_shared<Pool>();
    // 退出应用程序的函数，默认值为 `os.Exit()`
    exitFunc ExitFunc;
    // 用于格式化日志的缓冲池。如果为 nil，将使用默认的全局缓冲池。
    std::shared_ptr<BufferPool_interface> BufferPoolPtr;

    std::atomic<uint32_t> atomicLevel;

    // 默认构造函数
    Logger()
        : Out(std::make_shared<std::ofstream>()), // 默认输出到 stderr
          FormatterPtr(std::make_shared<TextFormatter>()), // 默认使用 TextFormatter
          currentLevel(std::make_shared<Level>(level::InfoLevel)), // 默认日志级别为 Info
          ExitFunc([](int code) { std::exit(code); }), // 默认退出函数
          atomicLevel(static_cast<uint32_t>(level::InfoLevel)) //默认初始化为InfoLevel
    {}

    std::shared_ptr<Entry> newEntry();
    void releaseEntry(std::shared_ptr<Entry> entry);    

    std::shared_ptr<Entry> WithField(const std::string& key, const std::string& value);
    std::shared_ptr<Entry> WithFields(const std::unordered_map<std::string, std::string>& fields);
    std::shared_ptr<Entry> WithError(const std::string& err);
    std::shared_ptr<Entry> WithContext(const std::shared_ptr<Context>& ctx);
    std::shared_ptr<Entry> WithTime(const std::chrono::system_clock::time_point& t);


    // 格式化日志
    void Logf(const Level& level, const std::string& format, ...);
    void Printf(const std::string& format, ...);

    void Tracef(const std::string& format, ...);
    void Debugf(const std::string& format, ...);
    void Infof(const std::string& format, ...);
    void Warnf(const std::string& format, ...);
    void Warningf(const std::string& format, ...);
    void Errorf(const std::string& format, ...);
    void Fatalf(const std::string& format, ...);
    void Panicf(const std::string& format, ...);

    void Log(Level level, const std::vector<boost::any>& args);
    void LogFn(Level level, LogFunction fn);

    void Print(const std::vector<boost::any>& args);
    void Trace(const std::vector<boost::any>& args);
    void Debug(const std::vector<boost::any>& args);
    void Info(const std::vector<boost::any>& args);
    void Warn(const std::vector<boost::any>& args);
    void Warning(const std::vector<boost::any>& args);
    void Error(const std::vector<boost::any>& args);
    void Fatal(const std::vector<boost::any>& args);
    void Panic(const std::vector<boost::any>& args);

    void PrintFn(LogFunction fn);
    void TraceFn(LogFunction fn);
    void DebugFn(LogFunction fn);
    void InfoFn(LogFunction fn);
    void WarnFn(LogFunction fn);
    void WarningFn(LogFunction fn);
    void ErrorFn(LogFunction fn);
    void FatalFn(LogFunction fn);
    void PanicFn(LogFunction fn);

    void Logln(const Level& level, const std::vector<boost::any>& args);
    void Println(const std::vector<boost::any>& args);
    void Traceln(const std::vector<boost::any>& args);
    void Debugln(const std::vector<boost::any>& args);
    void Infoln(const std::vector<boost::any>& args);
    void Warnln(const std::vector<boost::any>& args);
    void Warningln(const std::vector<boost::any>& args);
    void Errorln(const std::vector<boost::any>& args);
    void Fatalln(const std::vector<boost::any>& args);
    void Panicln(const std::vector<boost::any>& args);
    void Exit(int code);

    void SetNoLock();
    std::shared_ptr<Level> getlevel() const;
    void setLevel(Level newLevel);
    std::shared_ptr<Level> GetLevel() const;
    void AddHook(std::shared_ptr<Hook_interface> hook);
    bool IsLevelEnabled(const Level& level) const ;
    void SetFormatter(std::shared_ptr<Formatter_interface>& formatter);
    void SetOutput(const std::shared_ptr<std::ofstream>& output);
    void SetReportCaller(bool reportCaller);

    std::shared_ptr<LevelHooks> ReplaceHooks(const LevelHooks& newHooks);
    void SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool);
};

std::shared_ptr<Logger> New();
Level SetLelvel();

#endif