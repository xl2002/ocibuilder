#include "image/types/signature/policy_config.h"
#include "filesys/utils/homedir_unix.h"
#include "utils/cli/cli/common.h"



/**
 * @brief 获取默认签名验证策略
 * 
 * @param sys 系统上下文对象，包含配置信息
 * @return std::shared_ptr<Policy> 返回默认策略对象指针
 */
std::shared_ptr<Policy> DefaultPolicy(std::shared_ptr<SystemContext> sys){
    return NewPolicyFromFile(defaultPolicyPath(sys));
}

/**
 * @brief 获取默认策略文件路径
 * 
 * @param sys 系统上下文对象，包含配置信息
 * @return std::string 返回策略文件路径字符串
 */
std::string defaultPolicyPath(std::shared_ptr<SystemContext> sys){
    return defaultPolicyPathWithHomeDir(sys,homedir::Get());
}

/**
 * @brief 获取带主目录的默认策略路径
 * 
 * @param sys 系统上下文对象
 * @param homeDir 用户主目录路径
 * @return std::string 返回策略文件路径，按以下优先级：
 *         1. 系统上下文指定的路径
 *         2. 用户主目录下的策略文件
 *         3. 系统默认路径
 */
std::string defaultPolicyPathWithHomeDir(std::shared_ptr<SystemContext> sys,std::string homeDir){
    if(sys!=nullptr && sys->SignaturePolicyPath!=""){
        return sys->SignaturePolicyPath;
    }
    auto userPolicyFilePath=JoinPath(homeDir,userPolicyFile);
    if(fileExists(userPolicyFilePath)){
        return userPolicyFilePath;
    }
    if(sys!=nullptr && sys->RootForImplicitAbsolutePaths!=""){
        return joinPath(sys->RootForImplicitAbsolutePaths,systemDefaultPolicyPath);
    }
    return systemDefaultPolicyPath;
    // return "";
}

/**
 * @brief 从策略文件创建新策略对象
 * 
 * @param fileName 策略文件路径
 * @return std::shared_ptr<Policy> 返回策略对象指针
 * @note 当前实现返回nullptr，需要完善
 */
std::shared_ptr<Policy> NewPolicyFromFile(std::string fileName){

    return nullptr;
}
