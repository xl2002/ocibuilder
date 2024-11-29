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
/**
 * @brief 初始化 version 命令的内容
 * 
 */
void init_version(){
    versionOptions options;
    string name{"version"};
    string Short{"Display the Buildah version information"};
    string Long{"Displays Buildah version information."};
    string example{"buildah version [--json]"};
    Command* versionCommand=new Command{name,Short,Long,example};
    // versionCommand.Run=versionCmd;
    Flagset* flags=versionCommand->Flags();
    // flags.BoolVar();

    rootcmd.AddCommand({versionCommand});
    // return imagesCommand;
}

/**
 * @brief version 命令Run操作的
 * 
 */
void versionCmd(){

}