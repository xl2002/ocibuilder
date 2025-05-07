/**
 * @file exported.cpp
 * @brief 日志工具库的导出接口实现
 * @author [作者名]
 * @date 2025/4/9
 * @copyright [版权信息]
 */

#include "utils/logger/logrus/exported.h"

// std::shared_ptr<Logger> Std=New();

/**
 * @brief 设置日志级别
 * @param level 要设置的日志级别对象
 */
void SetLevel(std::shared_ptr<Level> level) {
    Std->setLevel(*level);
}
/**
 * @brief 获取当前日志级别
 * @return 当前日志级别对象
 */
std::shared_ptr<Level> GetLevel() {
    return Std->GetLevel();
}
// void Warnf(std::string format, ...) {
//     Std->Warnf(format);
// }
// void Warningf(std::string format, ...) {
//     Std->Warningf(format);
// }

/**
 * @brief 获取标准日志记录器
 * @return 标准日志记录器对象
 */
std::shared_ptr<Logger> StandardLogger(){
    return Std;
}
/**
 * @brief 设置日志输出流
 * @param output 要设置的输出流对象
 */
void SetOutput(const std::shared_ptr<std::ofstream>& output) {
    Std->SetOutput(output);
}
/**
 * @brief 设置日志格式化器
 * @param formatter 要设置的格式化器对象
 */
void SetFormatter(std::shared_ptr<Formatter_interface>& formatter) {
    Std->SetFormatter(formatter);
}
/**
 * @brief 设置缓冲区池
 * @param pool 要设置的缓冲区池对象
 */
void SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool) {
    Std->SetBufferPool(pool);
}
/**
 * @brief 设置是否报告调用者信息
 * @param reportCaller 是否报告调用者信息
 */
void SetReportCaller(bool reportCaller) {
    Std->SetReportCaller(reportCaller);
}
/**
 * @brief 检查指定日志级别是否启用
 * @param level 要检查的日志级别
 * @return 如果该级别日志启用则返回true，否则返回false
 */
bool IsLevelEnabled(const std::shared_ptr<Level>& level) {
    return Std->IsLevelEnabled(*level);
}
/**
 * @brief 添加日志钩子
 * @param hook 要添加的钩子对象
 */
void AddHook(const std::shared_ptr<Hook_interface>& hook) {
    Std->AddHook(hook);
}
/**
 * @brief 创建带有错误信息的日志条目
 * @param err 错误信息字符串
 * @return 包含错误信息的日志条目对象
 */
std::shared_ptr<Entry> WithError(const std::string& err) {
    return Std->WithError(err);
}
/**
 * @brief 创建带有上下文的日志条目
 * @param ctx 上下文对象
 * @return 包含上下文的日志条目对象
 */
std::shared_ptr<Entry> WithContext(const std::shared_ptr<Context>& ctx) {
    return Std->WithContext(ctx);
}
/**
 * @brief 创建带有多个字段的日志条目
 * @param fields 字段键值对映射
 * @return 包含字段的日志条目对象
 */
std::shared_ptr<Entry> WithFields(const std::unordered_map<std::string, std::string>& fields) {
    return Std->WithFields(fields);
}
/**
 * @brief 创建带有单个字段的日志条目
 * @param key 字段键
 * @param value 字段值
 * @return 包含字段的日志条目对象
 */
std::shared_ptr<Entry> WithField(const std::string& key, const std::string& value) {
    return Std->WithField(key, value);
}
/**
 * @brief 创建带有指定时间的日志条目
 * @param t 时间点
 * @return 包含指定时间的日志条目对象
 */
std::shared_ptr<Entry> WithTime(const std::chrono::system_clock::time_point& t) {
    return Std->WithTime(t);
}
/**
 * @brief 记录TRACE级别日志
 * @param args 日志参数列表
 */
void Trace(const std::vector<boost::any>& args) {
    Std->Trace(args);
}
/**
 * @brief 记录DEBUG级别日志
 * @param args 日志参数列表
 */
void Debug(const std::vector<boost::any>& args) {
    Std->Debug(args);
}
/**
 * @brief 记录INFO级别日志
 * @param args 日志参数列表
 */
void Info(const std::vector<boost::any>& args) {
    Std->Info(args);
}
/**
 * @brief 记录WARN级别日志
 * @param args 日志参数列表
 */
void Warn(const std::vector<boost::any>& args) {
    Std->Warn(args);
}
/**
 * @brief 记录WARNING级别日志
 * @param args 日志参数列表
 */
void Warning(const std::vector<boost::any>& args) {
    Std->Warning(args);
}
/**
 * @brief 记录ERROR级别日志
 * @param args 日志参数列表
 */
void Error(const std::vector<boost::any>& args) {
    Std->Error(args);
}
/**
 * @brief 记录PANIC级别日志
 * @param args 日志参数列表
 */
void Panic(const std::vector<boost::any>& args) {
    Std->Panic(args);
}
/**
 * @brief 记录FATAL级别日志
 * @param args 日志参数列表
 */
void Fatal(const std::vector<boost::any>& args) {
    Std->Fatal(args);
}
/**
 * @brief 记录TRACE级别日志（函数形式）
 * @param fn 日志函数
 */
void TraceFn(LogFunction fn) {
    Std->TraceFn(fn);
}
/**
 * @brief 记录DEBUG级别日志（函数形式）
 * @param fn 日志函数
 */
void DebugFn(LogFunction fn) {
    Std->DebugFn(fn);
}
/**
 * @brief 记录INFO级别日志（函数形式）
 * @param fn 日志函数
 */
void InfoFn(LogFunction fn) {
    Std->InfoFn(fn);
}
/**
 * @brief 记录WARN级别日志（函数形式）
 * @param fn 日志函数
 */
void WarnFn(LogFunction fn) {
    Std->WarnFn(fn);
}
/**
 * @brief 记录WARNING级别日志（函数形式）
 * @param fn 日志函数
 */
void WarningFn(LogFunction fn) {
    Std->WarningFn(fn);
}
/**
 * @brief 记录ERROR级别日志（函数形式）
 * @param fn 日志函数
 */
void ErrorFn(LogFunction fn) {
    Std->ErrorFn(fn);
}
/**
 * @brief 记录PANIC级别日志（函数形式）
 * @param fn 日志函数
 */
void PanicFn(LogFunction fn) {
    Std->PanicFn(fn);
}
/**
 * @brief 记录FATAL级别日志（函数形式）
 * @param fn 日志函数
 */
void FatalFn(LogFunction fn) {
    Std->FatalFn(fn);
}
/**
 * @brief 记录PRINT级别日志（函数形式）
 * @param fn 日志函数
 */
void PrintFn(LogFunction fn) {
    Std->PrintFn(fn);
}
/**
 * @brief 格式化记录TRACE级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Tracef(const std::string& format, ...) {
    Std->Tracef(format);
}
/**
 * @brief 格式化记录DEBUG级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Debugf(const std::string& format, ...) {
    Std->Debugf(format);
}
/**
 * @brief 格式化记录INFO级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Infof(const std::string& format, ...) {
    Std->Infof(format);
}
/**
 * @brief 格式化记录WARN级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Warnf(const std::string& format, ...) {
    Std->Warnf(format);
}
/**
 * @brief 格式化记录WARNING级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Warningf(const std::string& format, ...) {
    Std->Warningf(format);
}
/**
 * @brief 格式化记录ERROR级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Errorf(const std::string& format, ...) {
    Std->Errorf(format);
}
/**
 * @brief 格式化记录PANIC级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Panicf(const std::string& format, ...) {
    Std->Panicf(format);
}
/**
 * @brief 格式化记录FATAL级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Fatalf(const std::string& format, ...) {
    Std->Fatalf(format);
}
/**
 * @brief 格式化记录PRINT级别日志
 * @param format 格式化字符串
 * @param ... 格式化参数
 */
void Printf(const std::string& format, ...) {
    Std->Printf(format);
}
/**
 * @brief 记录TRACE级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Traceln(const std::vector<boost::any>& args) {
    Std->Traceln(args);
}
/**
 * @brief 记录DEBUG级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Debugln(const std::vector<boost::any>& args) {
    Std->Debugln(args);
}
/**
 * @brief 记录INFO级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Infoln(const std::vector<boost::any>& args) {
    Std->Infoln(args);
}
/**
 * @brief 记录WARN级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Warnln(const std::vector<boost::any>& args) {
    Std->Warnln(args);
}
/**
 * @brief 记录WARNING级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Warningln(const std::vector<boost::any>& args) {
    Std->Warningln(args);
}
/**
 * @brief 记录ERROR级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Errorln(const std::vector<boost::any>& args) {
    Std->Errorln(args);
}
/**
 * @brief 记录PANIC级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Panicln(const std::vector<boost::any>& args) {
    Std->Panicln(args);
}
/**
 * @brief 记录PRINT级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Println(const std::vector<boost::any>& args) {
    Std->Println(args);
}
/**
 * @brief 记录FATAL级别日志（换行结尾）
 * @param args 日志参数列表
 */
void Fatalln(const std::vector<boost::any>& args) {
    Std->Fatalln(args);
}
