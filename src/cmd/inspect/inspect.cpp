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
#include "cmd/inspect/inspect.h"
#include "utils/cli/cli/common.h"
/**
 * @brief 初始化 inspect 命令的内容
 * 
 */
void init_inspect(){
    string name{"inspect"};
    string Short{"Inspect the configuration of a container or image"};
    string Long{"Inspects a build container's or built image's configuration."};
    string example{"buildah inspect imageName/imageID"};
    Command* inspectCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    inspectCommand->SetUsageTemplate(Template);
    inspectCommand->Run=[=](Command& cmd, vector<string> args){
        inspectCmd(cmd,args);
    };
    rootcmd.AddCommand({inspectCommand});
}

/**
 * @brief inspect返回镜像的信息
 * 
 */
void inspectCmd(Command& cmd, vector<string> args){
    //1. 加载镜像仓库

    //2. 获得指定镜像的信息auto imagestore=store->Image(imageName/imageID);

    //3. 根据获得imagestore构造BuilderInfo，只需要填充OCI镜像部分

    //4. 打印镜像信息
}