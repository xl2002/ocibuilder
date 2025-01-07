/**
 * @file Config_Json.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief Config_Json.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "config/config.h"
#include "utils/common/go/string.h"
#include "filesys/system/selinux/selinux.h"
#include <boost/compute/detail/getenv.hpp>
// const std::string
// 	// _configPath is the path to the containers/containers.conf
// 	// inside a given config directory.
// 	_configPath = "containers/containers.conf",
// 	// UserOverrideContainersConfig holds the containers config path overridden by the rootless user
// 	UserOverrideContainersConfig = ".config/" + _configPath,
// 	// Token prefix for looking for helper binary under $BINDIR
// 	bindirPrefix = "$BINDIR";

void Config::CheckCgroupsAndAdjustConfig(){
    return;
}


// Helper function to check if the user is not root
bool userNotRoot(const std::string& user) {
    return user.empty() || user == "root" || user == "0";
}
std::vector<std::string> Config::Capabilities(const std::string& user,
                                    const std::vector<std::string>& addCapabilities,
                                    const std::vector<std::string>& dropCapabilities) {
    // std::vector<std::string> defaultCapabilities = Containers->DefaultCapabilities.Get();

    // if (userNotRoot(user)) {
    //     defaultCapabilities.clear();
    // }

    // std::vector<std::string> mergedCapabilities = mergeCapabilities(defaultCapabilities, addCapabilities, dropCapabilities);
    // return mergedCapabilities;
    return std::vector<std::string>();
}
std::vector<std::string> Config::Volumes(){
    return Containers->Volumes->Get();
}

std::vector<std::string> Config::GetDefaultEnv(){
    return GetDefaultEnvEx(Containers->EnvHost,Containers->HTTPProxy);
}
std::vector<std::string> Config::GetDefaultEnvEx(bool envHost,bool httpProxy){
    std::vector<std::string> env;
    if(envHost){
        env=env;
    }else{

    }
    auto Env=Containers->Env->Get();
    env.insert(env.end(),Env.begin(),Env.end());
    return env;
}
void Config::addCAPPrefix() {
    // 获取默认能力列表
    std::vector<std::string> caps = Containers->DefaultCapabilities->Get();
    std::vector<std::string> newCaps;
    newCaps.reserve(caps.size());

    for (const auto& val : caps) {
        std::string processedVal = val;
        // 如果 val 不以 "cap_" 开头，则添加前缀 "CAP_"
        if (toLower(processedVal).find("cap_") != 0) {
            processedVal = "CAP_" + toUpper(processedVal);
        }
        newCaps.push_back(processedVal);
    }
    // 设置新的能力列表
    Containers->DefaultCapabilities->Set(newCaps);
}
void Config::Validate(){
    if(!Containers->EnableLabeling){
        SetDisabled();
    }
}
// 设置环境变量的方法
void Config::setupEnv() {
    for (const auto& env : Engine->Env->Get()) {
        // 分割 KEY=value
        size_t pos = env.find('=');
        if (pos == std::string::npos) {
            // std::cerr << "invalid environment variable for engine " << env
            //             << ", valid configuration is KEY=value pair" << std::endl;
            continue;
        }

        std::string key = env.substr(0, pos);
        std::string value = env.substr(pos + 1);

        // 检查环境变量是否已定义
        if (boost::compute::detail::getenv(key.c_str()) != nullptr) {
            // std::cerr << "environment variable " << key
            //             << " is already defined, skip the settings from containers.conf" << std::endl;
            continue;
        }

        // 设置环境变量
        std::string envVar = key + "=" + value;
        // if (std::putenv(const_cast<char*>(envVar.c_str())) != 0) {
        //     throw std::runtime_error("failed to set environment variable " + key);
        // }
    }
}