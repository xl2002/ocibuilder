#include "image/buildah/buildah.h"

// std::string Builder::Architecture(){
//     return this->OCIv1->platform->Architecture;
// }
/**
 * @brief 初始化构建器配置
 * 
 * @param img 镜像接口指针
 * @param sys 系统上下文指针
 */
void Builder::initConfig(std::shared_ptr<Image_interface> img,std::shared_ptr<SystemContext> sys){
    
    if(img!=nullptr){
        
    }
}
/**
 * @brief 设置镜像注解
 * 
 * @param key 注解键
 * @param value 注解值
 */
void Builder::SetAnnotation(std::string key,std::string value){
    this->ImageAnnotations[key]=value;
}
/**
 * @brief 获取容器主机名
 * @return std::string 容器主机名
 */
std::string Builder::Hostname(){
    return this->Docker->config->Hostname;
}
/**
 * @brief 获取容器域名
 * @return std::string 容器域名
 */
std::string Builder::Domainname(){
    return this->Docker->config->Domainname;
}

/**
 * @brief 获取容器默认用户
 * @return std::string 用户名
 */
std::string Builder::User(){
    return this->OCIv1->config.user;
}
/**
 * @brief 获取容器环境变量
 * @return std::vector<std::string> 环境变量列表
 */
std::vector<std::string> Builder::Env(){
    return this->OCIv1->config.env;
}
/**
 * @brief 获取容器默认命令
 * @return std::vector<std::string> 命令列表
 */
std::vector<std::string> Builder::Cmd(){
    return this->OCIv1->config.cmd;
}
/**
 * @brief 获取容器工作目录
 * @return std::string 工作目录路径
 */
std::string Builder::WorkDir(){
    return this->OCIv1->config.workingDir;
}
/**
 * @brief 获取容器入口点命令
 * @return std::vector<std::string> 入口点命令列表
 */
std::vector<std::string> Builder::Entrypoint(){
    if(this->OCIv1->config.entrypoint.size()>0){
        return this->OCIv1->config.entrypoint;
    }
    return {};
}
/**
 * @brief 获取容器标签
 * @return std::map<std::string,std::string> 标签键值对
 */
std::map<std::string,std::string> Builder::Labels(){
    return this->OCIv1->config.labels;
}
/**
 * @brief 获取容器默认shell
 * @return std::vector<std::string> shell路径列表
 */
std::vector<std::string> Builder::Shell(){
    return this->Docker->config->Shell;
}

/**
 * @brief 获取容器停止信号
 * @return std::string 信号名称
 */
std::string Builder::StopSignal(){
    return this->Docker->config->StopSignal;
}
/**
 * @brief 获取容器OnBuild指令
 * @return std::vector<std::string> OnBuild指令列表
 */
std::vector<std::string> Builder::OnBuild(){
    return this->Docker->config->OnBuild;
}
/**
 * @brief 获取镜像维护者信息
 * @return std::string 维护者名称
 */
std::string Builder::Maintainer(){
    return this->OCIv1->author;
}
/**
 * @brief 获取镜像架构
 * @return std::string 架构名称(如amd64,arm64等)
 */
std::string Builder::Architecture(){
    return this->OCIv1->platform.Architecture;
}
