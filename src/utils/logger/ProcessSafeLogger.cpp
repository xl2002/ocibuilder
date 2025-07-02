#include "utils/logger/ProcessSafeLogger.h"
#include "storage/types/options.h"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
#include "utils/common/json.h"

const std::string DefaultConfigFileName = "config.json";
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
std::string GetConfigFilePath() {
    char userPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath))) {
        boost::filesystem::path configPath = boost::filesystem::path(userPath) / DefaultConfigFileName;
        return configPath.string();
    } else {
        throw std::runtime_error("无法获取用户目录");
    }
}
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
std::string GetConfigFilePath() {
    const char* homeDir = getenv("HOME");
    if (!homeDir) {
        struct passwd* pwd = getpwuid(getuid());
        if (pwd) homeDir = pwd->pw_dir;
    }
    if (homeDir) {
        boost::filesystem::path configPath = boost::filesystem::path(homeDir) / DefaultConfigFileName;
        return configPath.string();
    } else {
        throw std::runtime_error("无法获取用户目录");
    }
}
#endif

std::string getConfigPath()
{
    namespace fs = boost::filesystem;
    
    // 先检查当前目录
    fs::path currentPath = fs::current_path() / DefaultConfigFileName;
    if (fs::exists(currentPath)) {
        return currentPath.string();
    }
    
    // 检查用户目录
    try {
        fs::path userPath = fs::path(GetConfigFilePath()) / DefaultConfigFileName;
        return userPath.string();
    } catch (const std::runtime_error& e) {
        std::cerr << "警告: " << e.what() << "，将使用当前目录" << std::endl;
        return currentPath.string();
    }
}

const int DefaultMaxFiles = 7;
class LogConfig {
public:
    int max_files;
    LogConfig() : max_files(DefaultMaxFiles) {}
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const LogConfig& config) {
        jv = boost::json::object{
            {"max_files", config.max_files}
        };
    }
    friend LogConfig tag_invoke(boost::json::value_to_tag<LogConfig>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        LogConfig config;
        config.max_files = obj.at("max_files").as_int64();
        return config;
    }
};
int GetMaxFiles() {
    namespace fs = boost::filesystem;
    namespace json = boost::json;
    std::string configpath;
    int maxFiles = DefaultMaxFiles;
    try {
        configpath = getConfigPath();
    } catch (const std::runtime_error& e) {
        std::cerr << "无法获取配置文件路径: " << e.what() << std::endl;
        return maxFiles;
    }
    fs::path configPath(configpath);
    if (fs::exists(configPath)) {
        std::ifstream configFile(configPath.string());
        if (configFile.is_open()) {
            std::string configContent((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());
            LogConfig logconfig = unmarshal<LogConfig>(configContent);
            if(logconfig.max_files > 0) {
                maxFiles = logconfig.max_files;
            }
        } else {
            std::cerr << "无法打开配置文件: " << configPath.string() << std::endl;
        }
    }else{
        std::cerr << "配置文件不存在: " << configPath.string() << std::endl;
    }
    std::cout << "日志文件最大数量: " << maxFiles << std::endl;
    return maxFiles;
}
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
void ProcessSafeLogger::set_module(const std::string& Module){
    this->module=Module;
}


void ProcessSafeLogger::log_info(const std::string& message) {
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
    BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
    BOOST_LOG_TRIVIAL(info) << message;
}

void ProcessSafeLogger::log_warning(const std::string& message) {
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
    BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
    BOOST_LOG_TRIVIAL(warning) << message;
}

void ProcessSafeLogger::log_error(const std::string& message) {
    boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
    BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
    BOOST_LOG_TRIVIAL(error) << message;
}
void ProcessSafeLogger::init_logging() {
    namespace logging = boost::log;
    namespace sinks = boost::log::sinks;
    namespace expr = boost::log::expressions;
    namespace attrs = boost::log::attributes;

    // 文件名模板
    std::string file_pattern = log_dir_ + "/" + log_prefix_ + "_%Y-%m-%d.log";

    // 设置日志 sink
    boost::shared_ptr< sinks::synchronous_sink< sinks::text_file_backend > > sink =
        logging::add_file_log(
            logging::keywords::file_name = file_pattern,
            logging::keywords::open_mode = std::ios_base::app, // 关键修改：追加模式
            // logging::keywords::rotation_size = 10 * 1024 * 1024, // 单文件最大 10MB
            logging::keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // 每天 00:00
            logging::keywords::auto_flush = true
        );

    // 设置日志格式
    sink->set_formatter(
        expr::stream
            << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << logging::trivial::severity << "] "
            << "[pid:" << expr::attr<attrs::current_process_id::value_type>("ProcessID") << "] "
            << "[" << expr::attr<std::string>("Module") << "] "
            << expr::smessage
    );

    // 日志扫描旧文件并删除，最多保留最近 7 天
    sink->locked_backend()->set_file_collector(
        sinks::file::make_collector(
            logging::keywords::target = log_dir_,
            // logging::keywords::max_size = 100 * 1024 * 1024, // 所有文件总共最多 100MB
            // logging::keywords::min_free_space = 50 * 1024 * 1024, // 保留 50MB 可用空间
            logging::keywords::max_files = GetMaxFiles()                   // 默认保留 7 个文件（按天轮换）
        )
    );

    // 触发清理旧文件
    sink->locked_backend()->scan_for_files();

    // 添加属性
    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("ProcessID", attrs::current_process_id());
}