#include "image/buildah/buildah.h"

// std::string Builder::Architecture(){
//     return this->OCIv1->platform->Architecture;
// }
//读取镜像的config信息
void Builder::initConfig(std::shared_ptr<Image_interface> img,std::shared_ptr<SystemContext> sys){
    
    if(img!=nullptr){
        
    }
}
void Builder::SetAnnotation(std::string key,std::string value){
    this->ImageAnnotations[key]=value;
}
std::string Builder::Hostname(){
    return this->Docker->config->Hostname;
}
std::string Builder::Domainname(){
    return this->Docker->config->Domainname;
}

std::string Builder::User(){
    return this->OCIv1->config.user;
}
std::vector<std::string> Builder::Env(){
    return this->OCIv1->config.env;
}
std::vector<std::string> Builder::Cmd(){
    return this->OCIv1->config.cmd;
}
std::string Builder::WorkDir(){
    return this->OCIv1->config.workingDir;
}
std::vector<std::string> Builder::Entrypoint(){
    if(this->OCIv1->config.entrypoint.size()>0){
        return this->OCIv1->config.entrypoint;
    }
    return {};
}
std::map<std::string,std::string> Builder::Labels(){
    return this->OCIv1->config.labels;
}
std::vector<std::string> Builder::Shell(){
    return this->Docker->config->Shell;
}

std::string Builder::StopSignal(){
    return this->Docker->config->StopSignal;
}
std::vector<std::string> Builder::OnBuild(){
    return this->Docker->config->OnBuild;
}
std::string Builder::Maintainer(){
    return this->OCIv1->author;
}
std::string Builder::Architecture(){
    return this->OCIv1->platform.Architecture;
}