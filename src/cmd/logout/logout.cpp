/**
 * @file logout.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 logout.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/logout/logout.h"
#include "utils/cli/cli/common.h"
/**
 * @brief 初始化 logout 命令的内容
 * 
 */
void init_logout(){
    logoutOptions* options=new logoutOptions();
    string name{"logout"};
    string Short{"Logout of a container registry"};
    string Long{"Remove the cached username and password for the registry."};
    string example{"buildah logout quay.io"};
    Command* logoutCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    logoutCommand->SetUsageTemplate(Template);
    Flagset* flags=logoutCommand->Flags();
    flags->SetInterspersed(false);
    flags->BoolVar(options->all,"all",false,"remove all cached credentials");
    logoutCommand->Run=[=](Command& cmd, vector<string> args){
        logoutCmd(cmd,args,options);
    };
    rootcmd.AddCommand({logoutCommand});
    // return imagesCommand;
}

/**
 * @brief logout 命令Run操作的
 * 
 */
void logoutCmd(Command& cmd, vector<string> args, logoutOptions* iopts){
    //把auth文件清空
    saveLoginInfo("","");
    delete iopts;
}