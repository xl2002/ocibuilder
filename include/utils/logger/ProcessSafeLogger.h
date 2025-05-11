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

    void init_logging();

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
    void set_module(const std::string& Module);

    void log_info(const std::string& message);

    void log_warning(const std::string& message);

    void log_error(const std::string& message);
};
std::shared_ptr<ProcessSafeLogger> Newlogger();
extern std::shared_ptr<ProcessSafeLogger>logger;
extern std::string log_dir;
extern std::string log_prefix;

#endif // PROCESS_SAFE_LOGGER_HPP
