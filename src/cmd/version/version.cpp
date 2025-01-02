/**
 * @file version.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 version.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/version/version.h"
#include "utils/cli/cli/common.h"
#include "image/types/define/types.h"
#include <sstream>
#include "utils/common/json.h"
/**
 * @brief 初始化 version 命令的内容
 * 
 */
void init_version(){
    std::shared_ptr<versionOptions> options=std::make_shared<versionOptions>();
    string name{"version"};
    string Short{"Display the Buildah version information"};
    string Long{"Displays Buildah version information."};
    string example{"buildah version [--json]"};
    Command* versionCommand=new Command{name,Short,Long,example};
    string Template=UsageTemplate();
    versionCommand->SetUsageTemplate(Template);

    Flagset* flags=versionCommand->Flags();
    flags->BoolVar(options->json,"json",false,"Display the version information in JSON format");
    versionCommand->Run=[=](Command& cmd, vector<string> args){
        versionCmd(options);
    };
    rootcmd.AddCommand({versionCommand});
    // return imagesCommand;
}

/**
 * @brief version输出版本信息
 * 
 */
void versionCmd(std::shared_ptr<versionOptions> iopts){
    //1. 构建versionInfo对象，
    auto v=std::make_shared<versionInfo>();
    v->Version=version;
    ostringstream cppVersion;
    cppVersion<<"g++ "<<__GNUC__<<"."<<__GNUC_MINOR__<<"."<<__GNUC_PATCHLEVEL__;
    v->CppVersion=cppVersion.str();
    v->ImageSpec="1.0.1";
    v->Built="2025-01-01";
    v->OsArch="windows7/amd64";
    v->BuildPlatform="windows7/amd64";
    //2. 如果json=true，则输出json格式
    if(iopts->json){
        // std::string vstr=marshal<versionInfo>(*v);
        boost::json::value jv = boost::json::value_from(*v);
        std::cout<<format_json(jv)<<std::endl;
        // std::cout<<vstr<<std::endl;
    }else{
        std::cout<<std::left<<std::setw(20)<<"Version:"<<v->Version<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"C++ Version:"<<v->CppVersion<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Image Spec:"<<v->ImageSpec<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Built:"<<v->Built<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Os/Arch:"<<v->OsArch<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"BuildPlatform:"<<v->BuildPlatform<<std::endl;
    }
    return;
}