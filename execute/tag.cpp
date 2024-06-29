/**
 * @file tag.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 tag.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "tag.h"
/**
 * @brief 初始化 tag 命令的内容
 * 
 */
void init_tag(){
    string name{"tag"};
    string Short{"Add an additional name to a local image"};
    string Long{"Adds one or more additional names to locally-stored image."};
    string example{"buildah tag imageName firstNewName\n\
                    buildah tag imageName firstNewName SecondNewName"};
    Command tagCommand={name,Short,Long,example};
    tagCommand.Run=tagCmd;
    rootcmd.AddCommand({tagCommand});
}

/**
 * @brief tag 命令Run操作的
 * 
 */
void tagCmd(){

}