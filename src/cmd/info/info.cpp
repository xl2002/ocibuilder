/**
 * @file info.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 info.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/info/info.h"
/**
 * @brief 初始化 info 命令的内容
 * 
 */
void init_info(){
    string name{"info"};
    string Short{"Display Buildah system information"};
    string Long{"Display information about the host and current storage statistics which are useful when reporting issues."};
    string example{"buildah info"};
    Command infoCommand={name,Short,Long,example};
    // infoCommand.Run=infoCmd;
    rootcmd.AddCommand({&infoCommand});
}

/**
 * @brief info 命令Run操作的
 * 
 */
void infoCmd(){

}