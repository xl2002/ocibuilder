/**
 * @file push.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 push.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/push.h"
/**
 * @brief 初始化 push 命令的内容
 * 
 */
void init_push(){
    pushOptions options;
    string name{"push"};
    string Short{"Push an image to a specified destination"};
    string Long{"Pushes an image to a specified location."};
    string example{"buildah pull imagename\n\
                    buildah pull docker-daemon:imagename:imagetag\n\
                    buildah pull myregistry/myrepository/imagename:imagetag"};
    Command* pushCommand=new Command{name,Short,Long,example};
    // pushCommand.Run=pushCmd;
    Flagset* flags=pushCommand->Flags();
    // flags.StringVar();

    rootcmd.AddCommand({pushCommand});
    // return imagesCommand;
}

/**
 * @brief push 命令Run操作的
 * 
 */
void pushCmd(){

}