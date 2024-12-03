/**
 * @file rmi.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 rmi.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/rmi/rmi.h"
/**
 * @brief 初始化 rmi 命令的内容
 * 
 */
void init_rmi(){
    string name{"rmi"};
    string Short{"Remove one or more images from local storage"};
    string Long{"Removes one or more locally stored images."};
    string example{"buildah rmi imageID"};
    Command rmiCommand={name,Short,Long,example};
    // rmiCommand.Run=rmiCmd;
    rootcmd.AddCommand({&rmiCommand});
}

/**
 * @brief rmi 命令Run操作的
 * 
 */
void rmiCmd(){

}