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
/**
 * @brief 初始化 login 命令的内容
 * 
 */
void init_login(){
    LoginOptions options;
    string name{"login"};
    string Short{"Login to a container registry"};
    string Long{"Login to a container registry on a specified server."};
    string example{"buildah login quay.io"};
    Command* loginCommand=new Command(name,Short,Long,example);
    // loginCommand.Run=loginCmd;
    Flagset* flags=loginCommand->Flags();
    // flags.BoolVar();

    rootcmd.AddCommand({loginCommand});
    // return imagesCommand;
}

/**
 * @brief login 命令Run操作的
 * 
 */
void loginCmd(){

}