#if !defined(UTILS_LOGGER_LOGRUS_EXPORTED_H)
#define UTILS_LOGGER_LOGRUS_EXPORTED_H
#include "utils/logger/logrus/logger.h"

extern std::shared_ptr<Logger> Std;
void SetLevel(std::shared_ptr<Level> level);
std::shared_ptr<Level> GetLevel();
// void Warnf(std::string format, ...);
// void Warningf(std::string format, ...);
std::shared_ptr<Logger> StandardLogger();
void SetOutput(const std::shared_ptr<std::ofstream>& output);
void SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool);
void SetFormatter(std::shared_ptr<Formatter_interface>& formatter);
void SetReportCaller(bool reportCaller);
bool IsLevelEnabled(const std::shared_ptr<Level>& level);
void AddHook(const std::shared_ptr<Hook_interface>& hook);
std::shared_ptr<Entry> WithFields(const std::unordered_map<std::string, std::string>& fields);
std::shared_ptr<Entry> WithError(const std::string& err);
std::shared_ptr<Entry> WithContext(const std::shared_ptr<Context>& ctx);
std::shared_ptr<Entry> WithField(const std::string& key, const std::string& value);
std::shared_ptr<Entry> WithTime(const std::chrono::system_clock::time_point& t);
void Trace(const std::vector<boost::any>& args);
void Debug(const std::vector<boost::any>& args);
void Info(const std::vector<boost::any>& args);
void Warn(const std::vector<boost::any>& args);
void Warning(const std::vector<boost::any>& args);
void Error(const std::vector<boost::any>& args);
void Fatal(const std::vector<boost::any>& args);
void Panic(const std::vector<boost::any>& args);
void TraceFn(LogFunction fn);
void DebugFn(LogFunction fn);
void InfoFn(LogFunction fn);
void WarnFn(LogFunction fn);
void WarningFn(LogFunction fn);
void ErrorFn(LogFunction fn);
void FatalFn(LogFunction fn);
void PanicFn(LogFunction fn);
void PrintFn(LogFunction fn);
void Tracef (const std::string& format, ...);
void Debugf (const std::string& format, ...);
void Infof (const std::string& format, ...);
void Warnf (const std::string& format, ...);
void Warningf (const std::string& format, ...);
void Errorf (const std::string& format, ...);
void Fatalf (const std::string& format, ...);
void Panicf (const std::string& format, ...);
void Printf (const std::string& format, ...);
void Traceln(const std::vector<boost::any>& args);
void Debugln(const std::vector<boost::any>& args);
void Infoln(const std::vector<boost::any>& args);
void Warnln(const std::vector<boost::any>& args);
void Warningln(const std::vector<boost::any>& args);
void Errorln(const std::vector<boost::any>& args);
void Fatalln(const std::vector<boost::any>& args);
void Panicln(const std::vector<boost::any>& args);
void Println(const std::vector<boost::any>& args);

#endif // LOGRUS_EXPORTED_H)
