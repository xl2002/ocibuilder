/**
 * @file login.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 login.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/login/login.h"
#include "utils/cli/cli/common.h"
#include "storage/types/options.h"
/**
 * @brief 初始化 login 命令的内容
 * 
 */
void init_login(){
    LoginOptions* options=new LoginOptions();
    string name{"login"};
    string Short{"Login to a container registry"};
    string Long{"Login to a container registry on a specified server."};
    string example{"buildah login quay.io"};
    Command* loginCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    loginCommand->SetUsageTemplate(Template);
    Flagset* flags=loginCommand->Flags();
    flags->StringVar(options->username,"username","","username for the registry");
    flags->StringVar(options->password,"password","","password for the registry");
    flags->BoolVar(options->getLogin,"get-login",false,"get the login command for the registry");
    loginCommand->Run=[=](Command& cmd, vector<string> args){
        loginCmd(cmd,args,options);
    };
    rootcmd.AddCommand({loginCommand});
    // return imagesCommand;
}

/**
 * @brief login 命令Run操作的
 * 
 */
void loginCmd(Command& cmd, vector<string> args,LoginOptions* iopts){
    auto tmp=cmd.flags->actual_flags;
    std::string username=tmp["username"]->value->String();
    std::string password=tmp["password"]->value->String();
    saveLoginInfo(username,password);
    delete iopts;
}