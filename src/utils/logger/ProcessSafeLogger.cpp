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
            logging::keywords::max_files = 7                   // 最多保留 7 个文件（按天轮换）
        )
    );

    // 触发清理旧文件
    sink->locked_backend()->scan_for_files();

    // 添加属性
    logging::add_common_attributes();
    logging::core::get()->add_global_attribute("ProcessID", attrs::current_process_id());
}