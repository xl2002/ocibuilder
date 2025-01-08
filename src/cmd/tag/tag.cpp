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
#include "cmd/tag/tag.h"
#include "utils/cli/cli/common.h"
/**
 * @brief 初始化 tag 命令的内容
 * 
 */
void init_tag(){
    string name{"tag"};
    string Short{"Add an additional name to a local image"};
    string Long{"Adds one or more additional names to locally-stored image."};
    string example{"buildah tag imageName firstNewName\n  buildah tag imageName firstNewName SecondNewName"};
    Command* tagCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    tagCommand->SetUsageTemplate(Template);
    tagCommand->Args=MinimumNArgs(2);
    tagCommand->Run=[=](Command& cmd, vector<string> args){
        tagCmd(cmd,args);
    };
    rootcmd.AddCommand({tagCommand});
}

/**
 * @brief tag命令给镜像添加新的名称
 * 
 */
void tagCmd(Command& cmd, vector<string> args){
    //1. 加载镜像仓库
    auto store=getStore(&cmd);
    //2. 添加新的名称，通过imagestore->newtag(name,newname)实现

    auto images=store->image_store;
    try{
        images->newtag(args[0],args[1]);
    }catch(const myerror& e){
        e.logerror();
        exit(1);
    }
}