#include "utils/logger/logrus/exported.h"

// std::shared_ptr<Logger> Std=New();

void SetLevel(std::shared_ptr<Level> level) {
    Std->setLevel(*level);
}
std::shared_ptr<Level> GetLevel() {
    return Std->GetLevel();
}
// void Warnf(std::string format, ...) {
//     Std->Warnf(format);
// }
// void Warningf(std::string format, ...) {
//     Std->Warningf(format);
// }

std::shared_ptr<Logger> StandardLogger(){
    return Std;
}
void SetOutput(const std::shared_ptr<std::ofstream>& output) {
    Std->SetOutput(output);
}
void SetFormatter(std::shared_ptr<Formatter_interface>& formatter) {
    Std->SetFormatter(formatter);
}
void SetBufferPool(const std::shared_ptr<BufferPool_interface>& pool) {
    Std->SetBufferPool(pool);
}
void SetReportCaller(bool reportCaller) {
    Std->SetReportCaller(reportCaller);
}
bool IsLevelEnabled(const std::shared_ptr<Level>& level) {
    return Std->IsLevelEnabled(*level);
}
void AddHook(const std::shared_ptr<Hook_interface>& hook) {
    Std->AddHook(hook);
}
std::shared_ptr<Entry> WithError(const std::string& err) {
    return Std->WithError(err);
}
std::shared_ptr<Entry> WithContext(const std::shared_ptr<Context>& ctx) {
    return Std->WithContext(ctx);
}
std::shared_ptr<Entry> WithFields(const std::unordered_map<std::string, std::string>& fields) {
    return Std->WithFields(fields);
}
std::shared_ptr<Entry> WithField(const std::string& key, const std::string& value) {
    return Std->WithField(key, value);
}
std::shared_ptr<Entry> WithTime(const std::chrono::system_clock::time_point& t) {
    return Std->WithTime(t);
}
void Trace(const std::vector<boost::any>& args) {
    Std->Trace(args);
}
void Debug(const std::vector<boost::any>& args) {
    Std->Debug(args);
}
void Info(const std::vector<boost::any>& args) {
    Std->Info(args);
}
void Warn(const std::vector<boost::any>& args) {
    Std->Warn(args);
}
void Warning(const std::vector<boost::any>& args) {
    Std->Warning(args);
}
void Error(const std::vector<boost::any>& args) {
    Std->Error(args);
}
void Panic(const std::vector<boost::any>& args) {
    Std->Panic(args);
}
void Fatal(const std::vector<boost::any>& args) {
    Std->Fatal(args);
}
void TraceFn(LogFunction fn) {
    Std->TraceFn(fn);
}
void DebugFn(LogFunction fn) {
    Std->DebugFn(fn);
}
void InfoFn(LogFunction fn) {
    Std->InfoFn(fn);
}
void WarnFn(LogFunction fn) {
    Std->WarnFn(fn);
}
void WarningFn(LogFunction fn) {
    Std->WarningFn(fn);
}
void ErrorFn(LogFunction fn) {
    Std->ErrorFn(fn);
}
void PanicFn(LogFunction fn) {
    Std->PanicFn(fn);
}
void FatalFn(LogFunction fn) {
    Std->FatalFn(fn);
}
void PrintFn(LogFunction fn) {
    Std->PrintFn(fn);
}
void Tracef(const std::string& format, ...) {
    Std->Tracef(format);
}
void Debugf(const std::string& format, ...) {
    Std->Debugf(format);
}
void Infof(const std::string& format, ...) {
    Std->Infof(format);
}
void Warnf(const std::string& format, ...) {
    Std->Warnf(format);
}
void Warningf(const std::string& format, ...) {
    Std->Warningf(format);
}
void Errorf(const std::string& format, ...) {
    Std->Errorf(format);
}
void Panicf(const std::string& format, ...) {
    Std->Panicf(format);
}
void Fatalf(const std::string& format, ...) {
    Std->Fatalf(format);
}
void Printf(const std::string& format, ...) {
    Std->Printf(format);
}
void Traceln(const std::vector<boost::any>& args) {
    Std->Traceln(args);
}
void Debugln(const std::vector<boost::any>& args) {
    Std->Debugln(args);
}
void Infoln(const std::vector<boost::any>& args) {
    Std->Infoln(args);
}
void Warnln(const std::vector<boost::any>& args) {
    Std->Warnln(args);
}
void Warningln(const std::vector<boost::any>& args) {
    Std->Warningln(args);
}
void Errorln(const std::vector<boost::any>& args) {
    Std->Errorln(args);
}
void Panicln(const std::vector<boost::any>& args) {
    Std->Panicln(args);
}
void Println(const std::vector<boost::any>& args) {
    Std->Println(args);
}
void Fatalln(const std::vector<boost::any>& args) {
    Std->Fatalln(args);
}