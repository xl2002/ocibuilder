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
#include "utils/cli/cli/common.h"
#include "storage/storage/storage.h"
/**
 * @brief 初始化 info 命令的内容
 * 
 */
void init_info(){
    string name{"info"};
    string Short{"Display Buildah system information"};
    string Long{"Display information about the host and current storage statistics which are useful when reporting issues."};
    string example{"buildah info"};
    Command* infoCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    infoCommand->SetUsageTemplate(Template);
    infoCommand->Run=[=](Command& cmd, vector<string> args){
        infoCmd(cmd);
    };
    rootcmd.AddCommand({infoCommand});
}
/**
 * @brief 获取主机信息
 * 
 * @return std::map<std::string,std::string> 
 */
std::map<std::string,std::string> hostInfo(){
    //获取主机信息，不用那么详细，有些可以没有
    return {};
}
/**
 * @brief 获取存储信息
 * 
 * @param store 
 * @return std::map<std::string,std::string> 
 */
std::map<std::string,std::string> storeInfo(std::shared_ptr<Store> store){
    //获取存储信息，不用那么详细，有些可以没有
    return {};
}
/**
 * @brief 获取信息
 * 
 * @param store 
 * @return std::vector<InfoData> 
 */
std::vector<InfoData> Info(std::shared_ptr<Store> store){
    //1. 获取主机信息

    //2. 获取存储信息

    //3. 返回信息
    return {};
}
/**
 * @brief info 命令Run操作的
 * 
 */
void infoCmd(Command& cmd){
    //1. 获取镜像库

    //2. 获取信息

    //3. 格式化打印信息
}

