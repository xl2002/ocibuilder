#if !defined(CONFIG_NEW_H)
#define CONFIG_NEW_H
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <tuple>
#include "config/config.h"
#include "cobra/error.h"
// #include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/compute/detail/getenv.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/system/error_code.hpp>
#include <boost/nowide/utf/convert.hpp>
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/json_parser.hpp>
// Options类用于通过New()函数加载配置。
class Options {
public:
    // 尝试加载以下配置模块。
    std::vector<std::string> Modules;

    // 将加载的配置设置为默认配置，之后可以通过Default()访问。
    bool SetDefault=false;

    // 额外的配置文件加载。仅供内部使用的字段，用于在单元测试中使行为可观察和测试。
    std::vector<std::string> additionalConfigs;
    std::vector<std::string> modules();
};

std::shared_ptr<Config> Config_defaut();
std::shared_ptr<Config> newLocked(std::shared_ptr<Options> options);
std::tuple<std::string, boost::system::error_code> userConfigPath();
std::tuple<std::vector<std::string>, boost::system::error_code> systemConfigs();

std::pair<std::vector<std::string>,  boost::system::error_code>
addConfigs(const std::string& dirPath, std::vector<std::string>& configs);
std::tuple<std::string, boost::system::error_code> userConfigPath();
boost::optional<myerror> readConfigFromFile(const std::string& path, std::shared_ptr<Config> config, bool ignoreErrNotExist);

#endif // CONFIG_NEW_H
