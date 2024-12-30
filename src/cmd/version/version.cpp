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

    //3. 格式化输出，如果json=true，则输出json格式
}