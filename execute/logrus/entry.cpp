#include "utils/logger/logrus/entry.h"


std::shared_ptr<Entry> Entry::WithField(const std::string& key, const std::string& value) {
    std::unordered_map<std::string, std::string> fields{{key, value}};
    return WithFields(fields);
}

std::shared_ptr<Entry> Entry::WithFields(const std::unordered_map<std::string, std::string>& fields) {
    auto newEntry = std::make_shared<Entry>(*this);  // 复制当前 Entry
    for (const auto& f : fields) {
        auto k = f.first;
        auto v = f.second;
        newEntry->Data[k] = v;
    }
    return newEntry;
}

std::shared_ptr<Entry> Entry::WithError(const std::string& err) {
    return WithField("ErrorKey", err);
}

// std::shared_ptr<Entry> Entry::WithContext(const std::string& ctx) {
//     Data["context"] = ctx;
//     return shared_from_this();
// }
std::shared_ptr<Entry> Entry::WithContext(const std::shared_ptr<Context>& ctx) {
    // 创建新的 Entry，并复制当前对象的数据
    auto newEntry = std::make_shared<Entry>(*this);
    newEntry->ContextPtr = ctx;  // 设置新的上下文
    return newEntry;
}

std::shared_ptr<Entry> Entry::WithTime(const std::chrono::system_clock::time_point& time) {
    auto newEntry = std::make_shared<Entry>(*this);  // 复制当前 Entry
    newEntry->Time = time;
    return newEntry;
}

void Entry::Trace(const std::vector<boost::any>& args) {
    Log(level::TraceLevel, args);
}
void Entry::Debug(const std::vector<boost::any>& args) {
    Log(level::DebugLevel, args);
}
// Print 方法调用 Info 方法
void Entry::Print(const std::vector<boost::any>& args) {
    Info(args);
}
// Info 方法调用 Log 方法
void Entry::Info(const std::vector<boost::any>& args) {
    Log(level::InfoLevel, args);
}

void Entry::Warn(const std::vector<boost::any>& args) {
    Log(level::WarnLevel, args);
}

void Entry::Warning(const std::vector<boost::any>& args) {
    Warn(args);
}

void Entry::Error(const std::vector<boost::any>& args) {
    Log(level::ErrorLevel, args);
}

void Entry::Fatal(const std::vector<boost::any>& args) {
    Log(level::FatalLevel, args);
    LoggerPtr->Exit(1);
}

void Entry::Panic(const std::vector<boost::any>& args) {
    Log(level::PanicLevel, args);
}


//Logf 方法
void Entry::Logf(const Level& level, const std::string& format, ...) {
    if (LoggerPtr && LoggerPtr->IsLevelEnabled(level)) {
        // std::string formattedMessage = formatString(format, args);
        // log(level, formattedMessage);
        //获取变参
        char buffer[1024];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
        va_end(args);

        // 调用 Log 方法
        Log(level, std::vector<boost::any>{std::string(buffer)});
    }
}

// 主日志方法，接受任意参数并处理
void Entry::Log(const Level& level, const std::vector<boost::any>& args) {
    if (LoggerPtr->IsLevelEnabled(level)) {
        std::string message = formatArgs(args);
        log(level, message);
    }
}

// 
// 实际记录日志：：日志核心处理逻辑
// 日志核心处理逻辑
void Entry::log(const Level& level, const std::string& msg) {
    auto newEntry = Dup();

    if (Time == std::chrono::system_clock::time_point()) {
        Time = std::chrono::system_clock::now();
    }
    //设置日志级别与日志消息
    newEntry->LevelPtr = std::make_shared<Level>(level);
    newEntry->Message = msg;

    newEntry->LoggerPtr->mu->Lock();
    bool reportCaller = newEntry->LoggerPtr->ReportCaller;
    auto bufPool = newEntry->getBufferPool();
    newEntry->LoggerPtr->mu->Unlock();

    if (reportCaller) {
        newEntry->Caller = getCaller();  // 获取调用者信息
    }

    newEntry->fireHooks();
    auto buffer = bufPool->Get();
    // 使用智能指针管理缓冲区，并在作用域结束时自动将缓冲区放回缓冲池
    struct BufferGuard {
        std::shared_ptr<Entry> entry=std::make_shared<Entry>();
        std::shared_ptr<Buffer> buffer=std::make_shared<Buffer>();
        std::shared_ptr<BufferPool_interface> pool=nullptr;

        // ~BufferGuard() {
        //     // 延迟执行逻辑
        //     entry->BufferPtr = nullptr;
        //     buffer->Reset();
        //     pool->Put(std::move(buffer));
        // }
        ~BufferGuard()=default;
        BufferGuard(std::shared_ptr<Entry> entry, std::shared_ptr<Buffer> buffer, std::shared_ptr<BufferPool_interface> pool) 
                    : entry{std::move(entry)}, buffer{std::move(buffer)}, pool{std::move(pool)} {}
    };
    // 创建 BufferGuard 对象，确保在作用域结束时执行延迟操作
    BufferGuard guard{newEntry, std::move(buffer), bufPool};
    // 重置缓冲区
    guard.buffer->Reset();
    // 将缓冲区指针赋值给 Entry 对象
    newEntry->BufferPtr = std::move(guard.buffer);

    newEntry->write();

    if (level <= level::PanicLevel) {
        throw std::runtime_error("Panic Level Reached");  // 模拟 panic
    }
}

void Entry::Tracef(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::TraceLevel, format, args);
    va_end(args);
}
void Entry::Debugf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::DebugLevel, format, args);
    va_end(args);
}
// Infof 方法
void Entry::Infof(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::InfoLevel, format, args);
    va_end(args);
}

// Printf 方法
void Entry::Printf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Infof(format, args); // 调用 Infof
    va_end(args);
}
void Entry::Warnf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::WarnLevel, format, args);
    va_end(args);
}
void Entry::Warningf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Warnf( format, args);
    va_end(args);
}
void Entry::Errorf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::ErrorLevel, format, args);
    va_end(args);
}
void Entry::Fatalf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::FatalLevel, format, args);
    va_end(args);
    LoggerPtr->Exit(1);
}
void Entry::Panicf(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    Logf(level::PanicLevel, format, args);
    va_end(args);
}

// 模拟 Go 语言中的 Sprintlnn 功能，去除末尾的换行符
std::string Entry::sprintlnn(const std::vector<boost::any>& args) {
    std::ostringstream oss;

    // 遍历所有参数
    for (size_t i = 0; i < args.size(); ++i) {
        // 将每个参数添加到流中
        if (args[i].type() == typeid(int)) {
            oss << boost::any_cast<int>(args[i]);
        } else if (args[i].type() == typeid(double)) {
            oss << boost::any_cast<double>(args[i]);
        } else if (args[i].type() == typeid(std::string)) {
            oss << boost::any_cast<std::string>(args[i]);
        } else {
            oss << "[Unsupported type]";
        }

        // 在每个参数之间添加空格，但最后一个参数不需要空格
        if (i < args.size() - 1) {
            oss << " ";
        }
    }

    // 返回最终的字符串
    return oss.str();
}

void Entry::Logln(const Level& level, const std::vector<boost::any>& args) {
    if (LoggerPtr->IsLevelEnabled(level)) {
        std::string message = sprintlnn(args);
        Log(level, {message});
    }
}

void Entry::Traceln(const std::vector<boost::any> &args)
{
    Logln(level::TraceLevel, args);
}
void Entry::Debugln(const std::vector<boost::any> &args)
{
    Logln(level::DebugLevel, args);
}
void Entry::Infoln(const std::vector<boost::any> &args)
{
    Logln(level::InfoLevel, args);
}
void Entry::Println(const std::vector<boost::any> &args)
{
    Infoln(args);
}
void Entry::Warnln(const std::vector<boost::any> &args)
{
    Logln(level::WarnLevel, args);
}
void Entry::Warningln(const std::vector<boost::any> &args)
{
    Warnln(args);
}
void Entry::Errorln(const std::vector<boost::any> &args)
{
    Logln(level::ErrorLevel, args);
}
void Entry::Fatalln(const std::vector<boost::any> &args)
{
    Logln(level::FatalLevel, args);
    LoggerPtr->Exit(1);
}
void Entry::Panicln(const std::vector<boost::any> &args)
{
    Logln(level::PanicLevel, args);
}


// Dup 方法：创建当前 Entry 的副本
std::shared_ptr<Entry> Entry::Dup() {
    Fields newData = Data;  // 深拷贝 Data
    auto newEntry= std::make_shared<Entry>();

    newEntry->LoggerPtr = LoggerPtr;
    // newEntry->LevelPtr = LevelPtr;
    // newEntry->Message = Message;
    newEntry->ContextPtr = ContextPtr;
    // newEntry->BufferPtr = BufferPtr;
    newEntry->Time = Time;
    newEntry->Data = newData;
    newEntry->err = err;

    return newEntry;
    // return std::make_shared<Entry>(LoggerPtr, newData, Time, ContextPtr, err);
}
// 获取关联的 BufferPool_interface
std::shared_ptr<BufferPool_interface> Entry::getBufferPool() {
    if (LoggerPtr && LoggerPtr->BufferPoolPtr) {
        return LoggerPtr->BufferPoolPtr;
    }
    return globalBufferPool; // 返回默认的全局 BufferPool_interface
}
void Entry::fireHooks() {
    // 临时 LevelHooks 对象，用于复制 Logger 的钩子
    LevelHooks tmpHooks;

    // 锁定互斥锁，安全访问 Logger 的 Hooks
    {
        std::lock_guard<std::mutex> guard(LoggerPtr->mu->lock);
        tmpHooks = *LoggerPtr->Hooks;
    }

    // 触发对应级别的所有钩子
    try {
        auto s=shared_from_this();
        tmpHooks.Fire(*LevelPtr, s);
    } catch (const std::exception& e) {
        throw myerror("Hook_interface execution error: "+std::string(e.what()));
    }
}

void Entry::write() {
    std::lock_guard<std::mutex> guard(LoggerPtr->mu->lock);

    // 使用 FormatterPtr 对 Entry 进行格式化
    auto ret=LoggerPtr->FormatterPtr->Format(this);
    auto serialized = ret.first;
    auto errorMessage = ret.second;
    // auto [serialized, errorMessage] = LoggerPtr->FormatterPtr->Format(this);

    if (!errorMessage.empty()) {
        std::cerr << "Failed to obtain reader, " << errorMessage << std::endl;
        return;
    }

    // 写入日志内容
    try {
        if (LoggerPtr->Out && LoggerPtr->Out->is_open()) {
            LoggerPtr->Out->write(reinterpret_cast<const char*>(serialized.data()), serialized.size());
            if (LoggerPtr->Out->fail()) {
                std::cerr << "Failed to write to log." << std::endl;
            }
        } else {
            std::cerr << "Output stream is not open." << std::endl;
        }
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Failed to write to log, " << e.what() << std::endl;
    }
}

bool Entry::HasCaller(){
    return LoggerPtr != nullptr && LoggerPtr->ReportCaller && Caller != nullptr;
}

// 全局变量，用于缓存包名等信息
// std::string logrusPackage;
// int minimumCallerDepth = 1;  // 在堆栈初始化前的默认深度
// const int maximumCallerDepth = 25;
// const int knownLogrusFrames = 4;

// 使用 std::once_flag 保证初始化只执行一次
std::once_flag callerInitOnce;

// 获取包名（getPackageName）
std::string getPackageName(const std::string& functionName) {
    std::string result = functionName;
    while (true) {
        size_t lastPeriod = result.rfind(".");
        size_t lastColon = result.rfind("/");
        if (lastPeriod != std::string::npos && lastPeriod > lastColon) {
            result = result.substr(0, lastPeriod);
        } else {
            break;
        }
    }
    return result;
}

// 获取调用者信息（getCaller）
std::shared_ptr<Frame> getCaller() {
    // 初始化只执行一次，缓存包名和最小调用深度
    std::call_once(callerInitOnce, []() {
        boost::stacktrace::stacktrace st;
        for (size_t i = 0; i < st.size() && i < maximumCallerDepth; ++i) {
            auto frame = st[i];
            std::string funcName = frame.name();
            if (funcName.find("getCaller") != std::string::npos) {
                logrusPackage = getPackageName(funcName);
                break;
            }
        }
        minimumCallerDepth = knownLogrusFrames;
    });

    // 获取调用栈信息
    boost::stacktrace::stacktrace st;
    size_t depth = std::min(st.size(), static_cast<size_t>(maximumCallerDepth));


    for (size_t i = minimumCallerDepth; i < depth; ++i) {
        auto frame = st[i];
        std::string packageName = getPackageName(frame.name());

        // 如果调用者不属于缓存的包名，则返回该调用者信息
        //     if (packageName != logrusPackage) {
        //         return frame;
        //     }
        // }
        if (packageName != logrusPackage) {
            return std::make_shared<Frame>(
                reinterpret_cast<uintptr_t>(frame.address()), 
                nullptr,                                  // funcPtr is null, adjust as needed
                frame.name(),                            // 函数名
                frame.source_file(),                     // 文件名
                frame.source_line(),                     // 行号
                0,                                       // startLine, adjust if needed
                reinterpret_cast<uintptr_t>(frame.address())  // Entry point
            );
        }
    }

    // 如果未找到合适的调用者，返回第一个帧
    //return st[0];
    auto frame = st[0];
    return std::make_shared<Frame>(
        reinterpret_cast<uintptr_t>(frame.address()), 
        nullptr,                                  // funcPtr is null, adjust as needed
        frame.name(),                            // 函数名
        frame.source_file(),                     // 文件名
        frame.source_line(),                     // 行号
        0,                                       // startLine, adjust if needed
        reinterpret_cast<uintptr_t>(frame.address())  // Entry point
    );

}

// NewEntry 静态工厂方法实现
std::shared_ptr<Entry> Entry::NewEntry(std::shared_ptr<Logger> logger) {
    return std::make_shared<Entry>(std::move(logger));  // 创建并返回一个共享的 Entry 对象
}

std::pair<std::vector<uint8_t>, std::string> Entry::Bytes() {
    if (!LoggerPtr || !LoggerPtr->FormatterPtr) {
        // 返回空数据和错误消息，如果 Logger 或 Formatter 为空
        return {{}, "Logger or Formatter is not initialized."};
    }

    // 使用 Logger 的 Formatter 获取格式化的字节表示
    return LoggerPtr->FormatterPtr->Format(this);
}