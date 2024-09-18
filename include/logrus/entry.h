#if !defined(LOGRUS_ENTRY_H)
#define LOGRUS_ENTRY_H
#include <string>
#include <chrono>
#include <map>
#include <memory>
#include <sstream>
#include <ctime>
#include <any>
#include "logrus/logger.h"
#include "context/context.h"
#include "runtime_spec/runtime2.h"
#include "fmt/print.h"
#include <boost/stacktrace.hpp>

using Fields = std::unordered_map<std::string, std::any>;  // 用户设置的所有字段

class Entry : public std::enable_shared_from_this<Entry> {
public:
    std::shared_ptr<Logger> LoggerPtr;      // 日志器指针
    Fields Data;                         // 用户设置的所有字段
    std::chrono::system_clock::time_point Time;  // 日志创建的时间
    std::shared_ptr<Level> LevelPtr;        // 日志级别
    std::shared_ptr<Frame> Caller;       // 调用方法和包名
    std::string Message;                 // 日志消息
    std::shared_ptr<Buffer> BufferPtr;      // 日志缓冲区
    //Context Context;                     // 用户设置的上下文
    std::shared_ptr<Context> ContextPtr;
    std::string err;                     // 可能包含字段格式化错误

    // Entry()
    //     : Time(std::chrono::system_clock::now())  // 初始化日志时间
    // {
    // }
    Entry(std::shared_ptr<Logger> logger)
        : LoggerPtr(logger), Time(std::chrono::system_clock::now()),Data(6) {}

    void Reset() {
        Data.clear();
        LevelPtr.reset();
        Message.clear();
        BufferPtr.reset();
    }

    std::shared_ptr<Entry> WithField(const std::string& key, const std::string& value);
    std::shared_ptr<Entry> WithFields(const std::unordered_map<std::string, std::string>& fields);
    std::shared_ptr<Entry> WithError(const std::string& err);
//    std::shared_ptr<Entry> WithContext(const std::string& ctx);
    std::shared_ptr<Entry> WithContext(const std::shared_ptr<Context>& ctx);
    std::shared_ptr<Entry> WithTime(const std::chrono::system_clock::time_point& time);


    void Log(const Level& level, const std::vector<std::any>& args);
    void Trace(const std::vector<std::any>& args);
    void Debug(const std::vector<std::any>& args);
    void Print(const std::vector<std::any>& args);
    void Info(const std::vector<std::any>& args);
    void Warn(const std::vector<std::any>& args);
    void Warning(const std::vector<std::any>& args);
    void Error(const std::vector<std::any>& args);
    void Fatal(const std::vector<std::any>& args);
    void Panic(const std::vector<std::any>& args);

    //void Logf(const Level& level, const std::string& format, ...);
    void Logf(const Level& level, const std::string& format, ...);
    void Tracef(const std::string& format, ...);
    void Debugf(const std::string& format, ...);
    void Infof(const std::string& format, ...);
    void Printf(const std::string& format, ...);
    void Warnf(const std::string& format, ...);
    void Warningf(const std::string& format, ...);
    void Errorf(const std::string& format, ...);
    void Fatalf(const std::string& format, ...);
    void Panicf(const std::string& format, ...);

    void Logln(const Level& level, const std::vector<std::any>& args);
    void Traceln(const std::vector<std::any> &args);
    void Debugln(const std::vector<std::any> &args);
    void Infoln(const std::vector<std::any>& args);
    void Println(const std::vector<std::any>& args);
    void Warnln(const std::vector<std::any>& args);
    void Warningln(const std::vector<std::any>& args);
    void Errorln(const std::vector<std::any>& args);
    void Fatalln(const std::vector<std::any>& args);
    void Panicln(const std::vector<std::any>& args);

    std::shared_ptr<Entry> Dup() const;
    std::shared_ptr<BufferPool> getBufferPool();
    void fireHooks();
    void write();
    bool HasCaller() const;
    static std::shared_ptr<Entry> NewEntry(std::shared_ptr<Logger> logger);
    // 返回格式化后的字节表示
    std::pair<std::vector<uint8_t>, std::string> Bytes();

private:
    void log(const Level& level, const std::string& message);
    // 辅助：格式化字符串
    
    // 将 std::vector<std::any> 转换为字符串
    std::string formatArgs(const std::vector<std::any>& args) const {
        std::ostringstream oss;
        for (const auto& arg : args) {
            try {
                // 尝试转换每个参数为字符串
                oss << std::any_cast<std::string>(arg) << " ";
            } catch (const std::bad_any_cast&) {
                oss << "[Invalid argument type] ";
            }
        }
        return oss.str();
    }



};
extern std::string logrusPackage;
extern int minimumCallerDepth;
extern const int maximumCallerDepth;
extern const int knownLogrusFrames;
extern std::once_flag callerInitOnce;

std::string getPackageName(const std::string& funcName);
std::shared_ptr<Frame> getCaller();
#endif // LOGRUS_ENTRY_H