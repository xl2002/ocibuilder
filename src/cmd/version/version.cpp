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
#include "utils/logger/ProcessSafeLogger.h"
#include <sstream>
#include "filesys/systems.h"
#include <boost/predef.h>
#include "utils/common/json.h"
/**
 * @brief 初始化 version 命令的内容
 * @details 创建 version 命令对象，设置命令名称、描述、示例及 JSON 输出选项
 */
void init_version(){
    auto options=std::make_shared <versionOptions>();
    string name{"version"};
    string Short{"Display the ocibuilder version information"};
    string Long{"Displays ocibuilder version information."};
    string example{"ocibuilder version [--json]"}; 
    auto versionCommand=std::make_shared <Command>(name,Short,Long,example);
    string Template=UsageTemplate();
    versionCommand->SetUsageTemplate(Template);

    auto flags=versionCommand->Flags();
    flags->BoolVar(options->json,"json",false,"Display the version information in JSON format");
    versionCommand->Run=[=](std::shared_ptr<Command> cmd, vector<string> args){
        versionCmd(options);
    };
    rootcmd->AddCommand({versionCommand});
    // return imagesCommand;
}
/**
 * @brief 获取当前操作系统和 CPU 架构信息
 * @return std::string 返回格式为 "操作系统/架构" 的字符串（如 "Linux/x86_64"）
 */
std::string OSAndArch(){
    std::string os,arch;
    #if BOOST_OS_WINDOWS
        os="Windows";
    #elif BOOST_OS_LINUX
        os="Linux";
    #elif BOOST_OS_MACOS
        os="MacOS";
    #else
        os="Unknown";
    #endif

    // 架构
    #if BOOST_ARCH_X86_64
        arch="x86_64";
    #elif BOOST_ARCH_AMD64
        arch="amd64";
    #elif BOOST_ARCH_ARM
        arch="arm";
    #else
        arch="Unknown";
    #endif
    return os+"/"+arch;
}
/**
 * @brief 输出版本信息
 * @param iopts 版本命令选项，包含是否以 JSON 格式输出的标志
 * @details 根据参数决定以普通文本或 JSON 格式输出版本信息（包括编译器版本、构建时间等）
 */
void versionCmd(std::shared_ptr<versionOptions> iopts){
    logger->log_info("Start version command");
    //1. 构建versionInfo对象，
    auto v=std::make_shared<versionInfo>();
    v->Version=version;
    ostringstream cppVersion;
    cppVersion<<"g++ "<<__GNUC__<<"."<<__GNUC_MINOR__<<"."<<__GNUC_PATCHLEVEL__;
    v->CppVersion=cppVersion.str();
    v->ImageSpec="1.0.1";
    v->Built="2025-01-01";
    v->OsArch=OSAndArch();
    v->BuildPlatform="linux/amd64";
    //2. 如果json=true，则输出json格式
    if(iopts->json){
        // std::string vstr=marshal<versionInfo>(*v);
        boost::json::value jv = boost::json::value_from(*v);
        std::cout<<format_json(jv)<<std::endl;
        // std::cout<<vstr<<std::endl;
    }else{
        std::cout<<std::left<<std::setw(20)<<"Version:"<<v->Version<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Cpp Version:"<<v->CppVersion<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Image Spec:"<<v->ImageSpec<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Built:"<<v->Built<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"Os/Arch:"<<v->OsArch<<std::endl;
        std::cout<<std::left<<std::setw(20)<<"BuildPlatform:"<<v->BuildPlatform<<std::endl;
    }
    // delete iopts;
    logger->log_info("Version command completed successfully");
    return;
}
