#include "utils/logger/ProcessSafeLogger.h"
#include "storage/types/options.h"
std::shared_ptr<ProcessSafeLogger> Newlogger(){
    namespace fs = boost::filesystem;
    // std::string path =defaultRunRoot+log_dir;
    fs::path dir_path(log_dir);
    if (!fs::exists(dir_path)) {
        if (!fs::create_directories(dir_path)) {
            throw std::runtime_error("无法创建日志目录: " + log_dir);
        }
    }
    return std::make_shared<ProcessSafeLogger>(log_dir,log_prefix);
}