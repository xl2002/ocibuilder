#include "utils/logger/logrus/logger.h"

/**
 * @brief  SetLelvel
 * @return Level
 * @details Return an empty Level
 * @example
 * Level level = SetLelvel();
 * @note
 * This function is used by Logger to set the default log level
 * @author qcy
 * @date 2020-11-15
 */
Level SetLelvel()
{
    return Level();
}
std::shared_ptr<Logger> New() {
    auto logger = std::make_shared<Logger>();
    logger->Out = std::make_shared<std::ofstream>();  // 默认指向文件流，可以设置为其他流
    logger->FormatterPtr = std::make_shared<TextFormatter>();  // 默认格式化器
    logger->Hooks = std::make_shared<LevelHooks>();  // 初始化Hooks
    logger->currentLevel->lvl = level::InfoLevel;  // 设置默认日志级别为Info
    logger->atomicLevel = static_cast<uint32_t>(level::InfoLevel);
    logger->ReportCaller = false;  // 默认不记录调用者信息
    logger->ExitFunc = [](int code) { std::exit(code); };  // 默认退出函数
    logger->mu = std::make_shared<MutexWrap>();
    logger->entryPool = std::make_shared<Pool>();
    // logger->BufferPoolPtr = std::make_shared<BufferPool_interface>();
    return logger;
    // return std::make_shared<Logger>();
}

std::shared_ptr<Entry> Logger::newEntry() {
    std::lock_guard<std::mutex> Lock(mu->lock);
    if (!entryPool->empty()) {
        auto entry = entryPool->front();
        entryPool->pop();
        return entry;
    }
    return std::make_shared<Entry>(shared_from_this());
}

void Logger::releaseEntry(std::shared_ptr<Entry> entry) {
    entry->Reset();
    std::lock_guard<std::mutex> Lock(mu->lock);
    entryPool->push(entry);
}


std::shared_ptr<Entry> Logger::WithField(const std::string& key, const std::string& value) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithField(key, value);
}

std::shared_ptr<Entry> Logger::WithFields(const std::unordered_map<std::string, std::string>& fields) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithFields(fields);
}

std::shared_ptr<Entry> Logger::WithError(const std::string& err) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithError(err);
}

// std::shared_ptr<Entry> Logger::WithContext(const std::string& ctx) {
//     auto entry = newEntry();
//     releaseEntry(entry);
//     return entry->WithContext(ctx); 
// }
// 实现 Logger::WithContext 方法
std::shared_ptr<Entry> Logger::WithContext(const std::shared_ptr<Context>& ctx) { 
    auto entry = newEntry();// 获取一个新的 Entry
    auto newEntry = entry->WithContext(ctx); // 使用传入的上下文设置 Entry  
    releaseEntry(entry);// 释放原始 Entry
    return newEntry;// 返回包含新上下文的 Entry
}

std::shared_ptr<Entry> Logger::WithTime(const std::chrono::system_clock::time_point& t) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithTime(t);
}

// 实现 Logger::Logf
void Logger::Logf(const Level& level, const std::string& format, ...) {
    if (IsLevelEnabled(level)) {
        auto entry = newEntry();
        
        // 格式化可变参数
        va_list args;
        va_start(args, format);
        
        entry->Logf(level, format, args);
        releaseEntry(entry);
    }
}

//实现 Logger::Printf
void Logger::Printf(const std::string& format, ...) {
    auto entry = newEntry();

    va_list args;
    va_start(args, format);

    entry->Printf(format, args);
    releaseEntry(entry);
}

//退出函数
void Logger::Exit(int code) {
    runHandlers();

    if(!ExitFunc) {
        ExitFunc = [](int c) { std::exit(c); };
    }
    ExitFunc(code);
}

// 各级别日志
void Logger::Tracef(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::TraceLevel, format, args);
    va_end(args);
}

void Logger::Debugf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::DebugLevel, format, args);
    va_end(args);
}

void Logger::Infof(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::InfoLevel, format, args);
    va_end(args);
}

void Logger::Warnf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::WarnLevel, format, args);
    va_end(args);
}

void Logger::Warningf(const std::string& format, ...) {
    Warnf(format); // 调用 Warnf 方法
}

void Logger::Errorf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::ErrorLevel, format, args);
    va_end(args);
}

void Logger::Fatalf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::FatalLevel, format, args);
    va_end(args);
    Exit(1); // 退出程序
}

void Logger::Panicf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::PanicLevel, format, args);
    va_end(args);
}

// 实现 Log 方法，传递参数到 Entry::Log
void Logger::Log(Level level, const std::vector<boost::any>& args) {
    if (IsLevelEnabled(level)) {
        auto entry = newEntry();
        entry->Log(level, args);
        releaseEntry(entry);
    }
}
// 实现 LogFn 方法，调用 LogFunction 并传递参数
void Logger::LogFn(Level level, LogFunction fn) {
    if (IsLevelEnabled(level)) {
        auto entry = newEntry();
        entry->Log(level, fn());
        releaseEntry(entry);
    }
}

void Logger::Trace(const std::vector<boost::any>& args) {
    Log(level::TraceLevel, args);
}

void Logger::Debug(const std::vector<boost::any>& args) {
    Log(level::DebugLevel, args);
}

void Logger::Info(const std::vector<boost::any>& args) {
    Log(level::InfoLevel, args);
}

void Logger::Print(const std::vector<boost::any>& args) {
    auto entry = newEntry();
    entry->Print(args);
    releaseEntry(entry);
}

void Logger::Warn(const std::vector<boost::any>& args) {
    Log(level::WarnLevel, args);
}

void Logger::Warning(const std::vector<boost::any>& args) {
    Warn(args);  // Warning 调用 Warn
}

void Logger::Error(const std::vector<boost::any>& args) {
    Log(level::ErrorLevel, args);
}

void Logger::Fatal(const std::vector<boost::any>& args) {
    Log(level::FatalLevel, args);
    Exit(1);
}

void Logger::Panic(const std::vector<boost::any>& args) {
    Log(level::PanicLevel, args);
}

void Logger::PrintFn(LogFunction fn) {
    auto entry = newEntry();
    entry->Print(fn());
    releaseEntry(entry);
}

void Logger::TraceFn(LogFunction fn)
{
    LogFn(level::TraceLevel, fn);
}

void Logger::DebugFn(LogFunction fn)
{
    LogFn(level::DebugLevel, fn);
}

void Logger::InfoFn(LogFunction fn)
{
    LogFn(level::InfoLevel, fn);
}

void Logger::WarnFn(LogFunction fn)
{
    LogFn(level::WarnLevel, fn);
}

void Logger::WarningFn(LogFunction fn)
{
    WarnFn(fn);
}

void Logger::ErrorFn(LogFunction fn)
{
    LogFn(level::ErrorLevel, fn);
}

void Logger::FatalFn(LogFunction fn)
{
    LogFn(level::FatalLevel, fn);
    Exit(1);
}

void Logger::PanicFn(LogFunction fn)
{
    LogFn(level::PanicLevel, fn);
}

void Logger::Logln(const Level &level, const std::vector<boost::any> &args)
{
    if (IsLevelEnabled(level))
    {
        auto entry = newEntry();
        entry->Logln(level, args);
        releaseEntry(entry);
    }
}

void Logger::Println(const std::vector<boost::any> &args)
{
    auto entry = newEntry();
    entry->Println(args);
    releaseEntry(entry);
}

void Logger::Traceln(const std::vector<boost::any> &args)
{
    Logln(level::TraceLevel, args);
}

void Logger::Debugln(const std::vector<boost::any> &args)
{
    Logln(level::DebugLevel, args);
}

void Logger::Infoln(const std::vector<boost::any> &args)
{
    Logln(level::InfoLevel, args);
}

void Logger::Warnln(const std::vector<boost::any> &args)
{
    Logln(level::WarnLevel, args);
}

void Logger::Warningln(const std::vector<boost::any> &args)
{
    Warnln(args);
}

void Logger::Errorln(const std::vector<boost::any> &args)
{
    Logln(level::ErrorLevel, args);
}

void Logger::Fatalln(const std::vector<boost::any> &args)
{
    Logln(level::FatalLevel, args);
    Exit(1);
}

void Logger::Panicln(const std::vector<boost::any> &args)
{
    Logln(level::PanicLevel, args);
}


void Logger::SetNoLock() {
    mu->Disable();
}

// 当前日志级别
std::shared_ptr<Level> Logger::getlevel() const {
    auto lvl=std::make_shared<Level>();
    lvl->lvl = static_cast<level>(atomicLevel.load());
    return lvl;
    // return Level(static_cast<level>(atomicLevel.load())); // 原子读取日志级别
}
// Level Logger::getLevel() const {
//     //return Level(level::InfoLevel); // 需要实际的 std::atomic 实现
//     return currentLevel;
// }

// 设置日志级别
void Logger::setLevel(Level newLevel) {
    atomicLevel.store(static_cast<uint32_t>(newLevel.lvl)); // 原子存储新的日志级别
}

// 返回当前日志级别
std::shared_ptr<Level> Logger::GetLevel() const {
    return getlevel();  // 调用 level() 函数获取当前级别
}

// 添加钩子到 Logger 的 Hooks 中
void Logger::AddHook(std::shared_ptr<Hook_interface> hook) {
    // auto h=hook;
    mu->Lock();
    Hooks->Add(hook);
    mu->Unlock();
}

// 检查是否启用了指定的日志级别
bool Logger::IsLevelEnabled(const Level& level) const {
    auto lvl = getlevel();  // 获取当前级别
    return *lvl >= level;
}

void Logger::SetFormatter(std::shared_ptr<Formatter_interface>& formatter){
    mu->Lock();
    FormatterPtr = formatter;
    mu->Unlock();
}
void Logger::SetOutput(const std::shared_ptr<std::ofstream>& output){
    mu->Lock();
    Out = output;
    mu->Unlock();
}
void Logger::SetReportCaller(bool reportCaller){
    mu->Lock();
    ReportCaller = reportCaller;
    mu->Unlock();
}

std::shared_ptr<LevelHooks> Logger::ReplaceHooks(const LevelHooks& newHooks){
    mu->Lock();
    auto oldHooks = Hooks;
    Hooks = std::shared_ptr<LevelHooks>(new LevelHooks(newHooks));
    mu->Unlock();
    return oldHooks;
}
void Logger::SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool){
    mu->Lock();
    BufferPoolPtr = pool;
    mu->Unlock();
}
