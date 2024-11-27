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
#include "logrus/logger.h"
#include "logrus/bufferpool.h"
#include "boost/any.hpp"
using Fields = std::unordered_map<std::string, boost::any>;  // 用户设置的所有字段
class Logger;
class Entry : public std::enable_shared_from_this<Entry> {
public:
    std::shared_ptr<Logger> LoggerPtr=std::make_shared<Logger>();      // 日志器指针
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
    Entry() = default;
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
    std::string sprintlnn(const std::vector<boost::any>& args);

    void Log(const Level& level, const std::vector<boost::any>& args);
    void Trace(const std::vector<boost::any>& args);
    void Debug(const std::vector<boost::any>& args);
    void Print(const std::vector<boost::any>& args);
    void Info(const std::vector<boost::any>& args);
    void Warn(const std::vector<boost::any>& args);
    void Warning(const std::vector<boost::any>& args);
    void Error(const std::vector<boost::any>& args);
    void Fatal(const std::vector<boost::any>& args);
    void Panic(const std::vector<boost::any>& args);

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

    void Logln(const Level& level, const std::vector<boost::any>& args);
    void Traceln(const std::vector<boost::any> &args);
    void Debugln(const std::vector<boost::any> &args);
    void Infoln(const std::vector<boost::any>& args);
    void Println(const std::vector<boost::any>& args);
    void Warnln(const std::vector<boost::any>& args);
    void Warningln(const std::vector<boost::any>& args);
    void Errorln(const std::vector<boost::any>& args);
    void Fatalln(const std::vector<boost::any>& args);
    void Panicln(const std::vector<boost::any>& args);

    std::shared_ptr<Entry> Dup();
    std::shared_ptr<BufferPool_interface> getBufferPool();
    void fireHooks();
    void write();
    bool HasCaller() ;
    static std::shared_ptr<Entry> NewEntry(std::shared_ptr<Logger> logger);
    // 返回格式化后的字节表示
    std::pair<std::vector<uint8_t>, std::string> Bytes();

public:
    void log(const Level& level, const std::string& message);
    // 辅助：格式化字符串
    
    // 将 std::vector<boost::any> 转换为字符串
    std::string formatArgs(const std::vector<boost::any>& args) const {
        std::ostringstream oss;
        for (const auto& arg : args) {
            try {
                // 尝试转换每个参数为字符串
                oss << boost::any_cast<std::string>(arg) << " ";
            } catch (const boost::bad_any_cast&) {
                oss << "[Invalid argument type] ";
            }
        }
        return oss.str();
    }



};
extern std::string logrusPackage;
extern int minimumCallerDepth;
extern int maximumCallerDepth;
extern int knownLogrusFrames;
// extern std::once_flag callerInitOnce;

std::string getPackageName(const std::string& funcName);
std::shared_ptr<Frame> getCaller();
#endif // LOGRUS_ENTRY_H