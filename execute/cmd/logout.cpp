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
#include "cmd/logout.h"
/**
 * @brief 初始化 logout 命令的内容
 * 
 */
void init_logout(){
    logoutOptions options;
    string name{"logout"};
    string Short{"Logout of a container registry"};
    string Long{"Remove the cached username and password for the registry."};
    string example{"buildah logout quay.io"};
    Command* logoutCommand=new Command(name,Short,Long,example);
    // logoutCommand.Run=logoutCmd;
    Flagset* flags=logoutCommand->Flags();
    // flags.BoolVar();

    rootcmd.AddCommand({logoutCommand});
    // return imagesCommand;
}

/**
 * @brief logout 命令Run操作的
 * 
 */
void logoutCmd(){

}