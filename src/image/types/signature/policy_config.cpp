#include "image/types/signature/policy_config.h"
#include "filesys/utils/homedir_unix.h"
#include "utils/cli/cli/common.h"



std::shared_ptr<Policy> DefaultPolicy(std::shared_ptr<SystemContext> sys){
    return NewPolicyFromFile(defaultPolicyPath(sys));
}

std::string defaultPolicyPath(std::shared_ptr<SystemContext> sys){
    return defaultPolicyPathWithHomeDir(sys,homedir::Get());
}

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

std::shared_ptr<Policy> NewPolicyFromFile(std::string fileName){

    return nullptr;
}