#include "utils/logger/ProcessSafeLogger.h"
#include "storage/types/options.h"
#include <string>
#include <boost/filesystem.hpp>
#include <boost/json.hpp>
#include "utils/common/json.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

const std::string DefaultConfigFileName = "logConfig.json";
#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
std::string GetConfigFilePath() {
    char userPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, userPath))) {
        boost::filesystem::path configPath = boost::filesystem::path(userPath) / DefaultConfigFileName;
        return configPath.string();
    } else {
        throw std::runtime_error("Cannot get User Dir");
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
        throw std::runtime_error("Cannot get User Dir");
    }
}
#endif

std::string getConfigPath()
{
    namespace fs = boost::filesystem;
    
    // �ȼ�鵱ǰĿ¼
    fs::path currentPath = fs::current_path() / DefaultConfigFileName;
    if (fs::exists(currentPath)) {
        return currentPath.string();
    }
    
    // ����û�Ŀ¼
    try {
        fs::path userPath = fs::path(GetConfigFilePath());
        return userPath.string();
    } catch (const std::runtime_error& e) {
        std::cerr << "Warning: " << e.what() << ", use current directory" << std::endl;
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
        const auto& max_files_val = obj.at("max_files");
        if (max_files_val.is_int64())
            config.max_files = obj.at("max_files").as_int64();
        else if (max_files_val.is_string()) {
            try {
                config.max_files = std::stoi(max_files_val.as_string().c_str());
            } catch (const std::exception& e) {
                throw std::runtime_error("'max_files' must be a valid integer or integer string");
            }
        } else {
            throw std::runtime_error("'max_files' must be an integer or string");
        }
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
        std::cerr << "Cannot get the path of logConfig.json: " << e.what() << std::endl;
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
            std::cerr << "Cannot open logConfig.json: " << configPath.string() << std::endl;
        }
    }else{
        std::cerr << "logConfig.json does not exist: " << configPath.string() << std::endl;
        // ��������ļ������ڣ����ڵ�ǰĿ¼�´����µ������ļ�
        boost::property_tree::ptree pt;
        pt.put("max_files", maxFiles);
        try {
            boost::property_tree::write_json("logConfig.json", pt);
            std::cout << "Successfully created logConfig.json" << std::endl;
        } catch (const boost::property_tree::json_parser_error &e) {
            std::cerr << "Cannot write into logConfig.json" << e.what() << std::endl;
        }
    }
    std::cout << "max number of log file: " << maxFiles << std::endl;
    return maxFiles;
}
std::shared_ptr<ProcessSafeLogger> Newlogger(){
    namespace fs = boost::filesystem;
    // std::string path =defaultRunRoot+log_dir;
    fs::path dir_path(log_dir);
    if (!fs::exists(dir_path)) {
        if (!fs::create_directories(dir_path)) {
            throw std::runtime_error("cannot create log file dir: " + log_dir);
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

    // �ļ���ģ��
    std::string file_pattern = log_dir_ + "/" + log_prefix_ + "_%Y-%m-%d.log";

    // ������־ sink
    boost::shared_ptr< sinks::synchronous_sink< sinks::text_file_backend > > sink =
        logging::add_file_log(
            logging::keywords::file_name = file_pattern,
            logging::keywords::open_mode = std::ios_base::app, // �ؼ��޸ģ�׷��ģʽ
            // logging::keywords::rotation_size = 10 * 1024 * 1024, // ���ļ���� 10MB
            logging::keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // ÿ�� 00:00
            logging::keywords::auto_flush = true
        );

    // ������־��ʽ
    sink->set_formatter(
        expr::stream
            << "[" << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") << "] "
            << "[" << logging::trivial::severity << "] "
            << "[pid:" << expr::attr<attrs::current_process_id::value_type>("ProcessID") << "] "
            << "[" << expr::attr<std::string>("Module") << "] "
            << expr::smessage
    );

    // ��־ɨ����ļ���ɾ������ౣ����� 7 ��
    sink->locked_backend()->set_file_collector(
        sinks::file::make_collector(
            logging::keywords::target = log_dir_,
            // logging::keywords::max_size = 100 * 1024 * 1024, // �����ļ��ܹ���� 100MB
            // logging::keywords::min_free_space = 50 * 1024 * 1024, // ���� 50MB ���ÿռ�
            logging::keywords::max_files = GetMaxFiles()                   // Ĭ�ϱ��� 7 ���ļ��������ֻ���
        )
    );

    // ����������ļ�
    sink->locked_backend()->scan_for_files();

    // �������
    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("ProcessID", attrs::current_process_id());
}