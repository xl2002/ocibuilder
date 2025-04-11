#include "utils/logger/logrus/logger.h"

/**
 * @file logger.cpp
 * @brief Logger类实现文件
 * @details 实现了Logger类及其相关功能，包括日志级别设置、日志记录、格式化输出等
 * @author qcy
 * @date 2020-11-15
 */

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
/**
 * @brief 创建新的Logger实例
 * @return std::shared_ptr<Logger> 新创建的Logger共享指针
 * @details 初始化Logger的默认属性：
 * - 输出到文件流
 * - 使用文本格式化器
 * - 默认日志级别为Info
 * - 不记录调用者信息
 * - 使用标准退出函数
 */
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

/**
 * @brief 创建新的日志条目
 * @return std::shared_ptr<Entry> 新创建的Entry共享指针
 * @details 从条目池中获取或创建新的Entry实例
 * @note 线程安全，使用互斥锁保护
 */
std::shared_ptr<Entry> Logger::newEntry() {
    std::lock_guard<std::mutex> Lock(mu->lock);
    if (!entryPool->empty()) {
        auto entry = entryPool->front();
        entryPool->pop();
        return entry;
    }
    return std::make_shared<Entry>(shared_from_this());
}

/**
 * @brief 释放日志条目回池
 * @param entry 要释放的Entry共享指针
 * @details 重置Entry状态并将其放回池中以供重用
 * @note 线程安全，使用互斥锁保护
 */
void Logger::releaseEntry(std::shared_ptr<Entry> entry) {
    entry->Reset();
    std::lock_guard<std::mutex> Lock(mu->lock);
    entryPool->push(entry);
}


/**
 * @brief 添加单个字段到日志条目
 * @param key 字段键
 * @param value 字段值
 * @return std::shared_ptr<Entry> 包含新字段的Entry共享指针
 * @details 创建新Entry并添加指定字段
 */
std::shared_ptr<Entry> Logger::WithField(const std::string& key, const std::string& value) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithField(key, value);
}

/**
 * @brief 添加多个字段到日志条目
 * @param fields 字段映射表
 * @return std::shared_ptr<Entry> 包含新字段的Entry共享指针
 * @details 创建新Entry并添加指定字段集合
 */
std::shared_ptr<Entry> Logger::WithFields(const std::unordered_map<std::string, std::string>& fields) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithFields(fields);
}

/**
 * @brief 添加错误信息到日志条目
 * @param err 错误信息字符串
 * @return std::shared_ptr<Entry> 包含错误信息的Entry共享指针
 * @details 创建新Entry并添加错误信息字段
 */
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
/**
 * @brief 添加上下文到日志条目
 * @param ctx 上下文共享指针
 * @return std::shared_ptr<Entry> 包含上下文的Entry共享指针
 * @details 创建新Entry并添加上下文信息
 */
std::shared_ptr<Entry> Logger::WithContext(const std::shared_ptr<Context>& ctx) { 
    auto entry = newEntry();// 获取一个新的 Entry
    auto newEntry = entry->WithContext(ctx); // 使用传入的上下文设置 Entry  
    releaseEntry(entry);// 释放原始 Entry
    return newEntry;// 返回包含新上下文的 Entry
}

/**
 * @brief 设置日志条目的自定义时间
 * @param t 系统时钟时间点
 * @return std::shared_ptr<Entry> 包含自定义时间的Entry共享指针
 * @details 创建新Entry并设置指定的时间戳
 */
std::shared_ptr<Entry> Logger::WithTime(const std::chrono::system_clock::time_point& t) {
    auto entry = newEntry();
    releaseEntry(entry);
    return entry->WithTime(t);
}

// 实现 Logger::Logf
/**
 * @brief 格式化日志记录
 * @param level 日志级别
 * @param format 格式化字符串
 * @details 使用可变参数格式化日志消息并记录
 * @note 如果日志级别未启用则不记录
 */
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
/**
 * @brief 格式化打印
 * @param format 格式化字符串
 * @details 使用可变参数格式化消息并打印
 */
void Logger::Printf(const std::string& format, ...) {
    auto entry = newEntry();

    va_list args;
    va_start(args, format);

    entry->Printf(format, args);
    releaseEntry(entry);
}

//退出函数
/**
 * @brief 退出程序
 * @param code 退出码
 * @details 运行所有处理器后调用退出函数
 * @note 如果未设置退出函数，则使用标准退出
 */
void Logger::Exit(int code) {
    runHandlers();

    if(!ExitFunc) {
        ExitFunc = [](int c) { std::exit(c); };
    }
    ExitFunc(code);
}

// 各级别日志
/**
 * @brief 记录Trace级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Tracef(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::TraceLevel, format, args);
    va_end(args);
}

/**
 * @brief 记录Debug级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Debugf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::DebugLevel, format, args);
    va_end(args);
}

/**
 * @brief 记录Info级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Infof(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::InfoLevel, format, args);
    va_end(args);
}

/**
 * @brief 记录Warn级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Warnf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::WarnLevel, format, args);
    va_end(args);
}

/**
 * @brief 记录Warning级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 调用Warnf方法实现
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Warningf(const std::string& format, ...) {
    Warnf(format); // 调用 Warnf 方法
}

/**
 * @brief 记录Error级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Errorf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::ErrorLevel, format, args);
    va_end(args);
}

/**
 * @brief 记录Fatal级别日志(格式化字符串)并退出程序
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录，然后调用Exit(1)退出程序
 * @note 线程安全，内部使用互斥锁保护
 */
void Logger::Fatalf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::FatalLevel, format, args);
    va_end(args);
    Exit(1); // 退出程序
}

/**
 * @brief 记录Panic级别日志(格式化字符串)
 * @param format 格式化字符串
 * @param ... 可变参数
 * @details 使用可变参数格式化日志消息并记录
 * @note 线程安全，内部使用互斥锁保护
 */
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


/**
 * @brief 禁用日志器的线程安全锁
 * @details 调用此方法后，日志器将不再保证线程安全
 * @warning 仅在单线程环境下使用或确保外部同步时调用
 */
void Logger::SetNoLock() {
    mu->Disable();
}

// 当前日志级别
/**
 * @brief 获取当前日志级别
 * @return std::shared_ptr<Level> 当前日志级别共享指针
 * @details 原子读取当前日志级别并返回
 * @note 线程安全
 */
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
/**
 * @brief 设置日志级别
 * @param newLevel 新的日志级别
 * @details 原子设置新的日志级别
 * @note 线程安全
 */
void Logger::setLevel(Level newLevel) {
    atomicLevel.store(static_cast<uint32_t>(newLevel.lvl)); // 原子存储新的日志级别
}

// 返回当前日志级别
/**
 * @brief 获取当前日志级别(公开接口)
 * @return std::shared_ptr<Level> 当前日志级别共享指针
 * @details 调用getlevel()获取当前级别
 * @note 线程安全
 */
std::shared_ptr<Level> Logger::GetLevel() const {
    return getlevel();  // 调用 level() 函数获取当前级别
}

// 添加钩子到 Logger 的 Hooks 中
/**
 * @brief 添加日志钩子
 * @param hook 钩子接口共享指针
 * @details 将钩子添加到日志器的钩子列表中
 * @note 线程安全，使用互斥锁保护
 */
void Logger::AddHook(std::shared_ptr<Hook_interface> hook) {
    // auto h=hook;
    mu->Lock();
    Hooks->Add(hook);
    mu->Unlock();
}

// 检查是否启用了指定的日志级别
/**
 * @brief 检查指定日志级别是否启用
 * @param level 要检查的日志级别
 * @return bool 如果级别启用返回true，否则false
 * @details 比较当前日志级别与指定级别
 * @note 线程安全
 */
bool Logger::IsLevelEnabled(const Level& level) const {
    auto lvl = getlevel();  // 获取当前级别
    return *lvl >= level;
}

/**
 * @brief 设置日志格式化器
 * @param formatter 格式化器接口共享指针
 * @details 设置日志器使用的格式化器
 * @note 线程安全，使用互斥锁保护
 */
void Logger::SetFormatter(std::shared_ptr<Formatter_interface>& formatter){
    mu->Lock();
    FormatterPtr = formatter;
    mu->Unlock();
}
/**
 * @brief 设置日志输出流
 * @param output 输出流共享指针
 * @details 设置日志器使用的输出流
 * @note 线程安全，使用互斥锁保护
 */
void Logger::SetOutput(const std::shared_ptr<std::ofstream>& output){
    mu->Lock();
    Out = output;
    mu->Unlock();
}
/**
 * @brief 设置是否报告调用者信息
 * @param reportCaller 是否报告调用者
 * @details 设置日志器是否记录调用者信息
 * @note 线程安全，使用互斥锁保护
 */
void Logger::SetReportCaller(bool reportCaller){
    mu->Lock();
    ReportCaller = reportCaller;
    mu->Unlock();
}

/**
 * @brief 替换日志钩子集合
 * @param newHooks 新的钩子集合
 * @return std::shared_ptr<LevelHooks> 被替换的旧钩子集合
 * @details 完全替换日志器的钩子集合
 * @note 线程安全，使用互斥锁保护
 */
std::shared_ptr<LevelHooks> Logger::ReplaceHooks(const LevelHooks& newHooks){
    mu->Lock();
    auto oldHooks = Hooks;
    Hooks = std::shared_ptr<LevelHooks>(new LevelHooks(newHooks));
    mu->Unlock();
    return oldHooks;
}
/**
 * @brief 设置缓冲区池
 * @param pool 缓冲区池接口共享指针
 * @details 设置日志器使用的缓冲区池
 * @note 线程安全，使用互斥锁保护
 */
void Logger::SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool){
    mu->Lock();
    BufferPoolPtr = pool;
    mu->Unlock();
}
