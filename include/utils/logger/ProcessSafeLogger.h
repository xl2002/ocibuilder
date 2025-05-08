#ifndef PROCESS_SAFE_LOGGER_HPP
#define PROCESS_SAFE_LOGGER_HPP

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/current_process_id.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <stdexcept>
#include <fstream>
#include <string>
#include <memory>
class ProcessSafeLogger {
private:
    std::string log_dir_;
    std::string log_prefix_;
    std::string module;
    std::shared_ptr<boost::interprocess::file_lock> file_lock_;

    void init_logging() {
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
                logging::keywords::rotation_size = 10 * 1024 * 1024, // 单文件最大 10MB
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
                logging::keywords::max_size = 100 * 1024 * 1024, // 所有文件总共最多 100MB
                logging::keywords::min_free_space = 50 * 1024 * 1024, // 保留 50MB 可用空间
                logging::keywords::max_files = 7                   // 最多保留 7 个文件（按天轮换）
            )
        );

        // 触发清理旧文件
        sink->locked_backend()->scan_for_files();

        // 添加属性
        logging::add_common_attributes();
        logging::core::get()->add_global_attribute("ProcessID", attrs::current_process_id());
    }

public:
    explicit ProcessSafeLogger(const std::string& log_dir, const std::string& log_prefix)
        : log_dir_(log_dir), log_prefix_(log_prefix) {
        // 确保日志目录存在
        std::ofstream dummy(log_dir_ + "/.touch", std::ios::app);
        dummy.close();

        // 使用第一个日志文件作为锁对象
        std::string lock_file = log_dir_ + "/" + log_prefix_ + "_lock.log";
        std::ofstream touch(lock_file, std::ios::app); touch.close();
        file_lock_ = std::make_shared<boost::interprocess::file_lock>(lock_file.c_str());

        init_logging();
    }
    void set_module(const std::string& Module){
        this->module=Module;
    }

    void log_info(const std::string& message) {
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
        BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
        BOOST_LOG_TRIVIAL(info) << message;
    }

    void log_warning(const std::string& message) {
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
        BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
        BOOST_LOG_TRIVIAL(warning) << message;
    }

    void log_error(const std::string& message) {
        boost::interprocess::scoped_lock<boost::interprocess::file_lock> lock(*file_lock_);
        BOOST_LOG_SCOPED_THREAD_TAG("Module", module);
        BOOST_LOG_TRIVIAL(error) << message;
    }
};
std::shared_ptr<ProcessSafeLogger> Newlogger();
extern std::shared_ptr<ProcessSafeLogger>logger;
extern std::string log_dir;
extern std::string log_prefix;

#endif // PROCESS_SAFE_LOGGER_HPP
