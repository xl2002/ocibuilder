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
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 初始化 tag 命令的内容
 * @details 创建并配置 tag 命令对象，设置命令名称、描述、示例及参数要求。
 *          该命令用于为本地镜像添加一个或多个新名称。
 */
void init_tag(){
    string name{"tag"};
    string Short{"Add an additional name to a local image"};
    string Long{"Adds one or more additional names to locally-stored image."};
    string example{"ocibuilder tag imageName firstNewName\n  ocibuilder tag imageName firstNewName SecondNewName"};
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
 * @brief 执行 tag 命令，为镜像添加新名称
 * @param cmd  Command 对象，包含命令上下文信息
 * @param args 命令行参数列表，至少包含两个参数：
 *             - args[0]: 原始镜像名称
 *             - args[1]: 要添加的新名称
 * @details 通过镜像存储库的 `newtag` 方法实现名称添加，若失败则输出错误并退出。
 */
void tagCmd(Command& cmd, vector<string> args){
    logger->log_info("Start tag command for image: " + args[0]);
    //1. 加载镜像仓库
    auto store=getStore(&cmd);
    //2. 添加新的名称，通过imagestore->newtag(name,newname)实现

    auto images=store->image_store;
    try{
        images->newtag(args[0],args[1]);
        logger->log_info("Successfully tagged " + args[0] + " as " + args[1]);
    }catch(const myerror& e){
        logger->log_error("Failed to tag image: " + std::string(e.what()));
        e.logerror();
        exit(1);
    }
}
