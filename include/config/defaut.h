#if !defined(CONFIG_DEFAUT_H)
#define CONFIG_DEFAUT_H
#include <string>
#include <vector>
#include <memory>
#include <tuple>

#include "config/config.h"
#include "utils/common/error.h"
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

std::shared_ptr<Config> defaultConfig();
std::tuple<std::shared_ptr<EngineConfig>,boost::optional<myerror>> defaultEngineConfig();
std::tuple<std::string,boost::optional<myerror>> defaultTmpDir();
std::vector<std::string> getDefaultComposeProviders();
// std::vector<std::string> getDefaultMachineVolumes();
std::shared_ptr<SecretConfig> defaultSecretConfig();
std::shared_ptr<MachineConfig> defaultMachineConfig();
std::shared_ptr<FarmConfig> defaultFarmConfig();
std::string getDefaultTmpDir();
std::string defaultCgroupManager();
std::string defaultEventsLogger();
std::string defaultLogDriver();
std::string getDefaultSSHConfig();
SubnetPool parseSubnetPool(const std::string& subnet, int size);
std::string getDefaultMachineUser();







#endif // CONF
