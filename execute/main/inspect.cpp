/**
 * @file inspect.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 inspect.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "main/inspect.h"
/**
 * @brief 初始化 inspect 命令的内容
 * 
 */
void init_inspect(){
    string name{"inspect"};
    string Short{"Inspect the configuration of a container or image"};
    string Long{"Inspects a build container's or built image's configuration."};
    string example{"buildah inspect"};
    Command inspectCommand={name,Short,Long,example};
    // inspectCommand.Run=inspectCmd;
    rootcmd.AddCommand({&inspectCommand});
}

/**
 * @brief inspect 命令Run操作的
 * 
 */
void inspectCmd(){

}