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
#include "utils/cli/cli/common.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 初始化 rmi 命令的内容
 * 
 */
void init_rmi(){
    string name{"rmi"};
    string Short{"Remove one or more images from local storage"};
    string Long{"Removes one or more locally stored images."};
    string example{"ocibuilder rmi imageID"};
    auto  rmiCommand=std::make_shared <Command>(name,Short,Long,example);
    string Template=UsageTemplate();
    rmiCommand->SetUsageTemplate(Template);
    auto flags=rmiCommand->Flags();
    flags->SetInterspersed(false);
    rmiCommand->Run=[=](std::shared_ptr<Command> cmd, vector<string> args){
        rmiCmd(cmd,args);
    };
    rootcmd->AddCommand({rmiCommand});
}

// bool isHexChar(char ch) {
//     return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
// }
// bool isHexadecimal(const std::string& arg) {
//     return std::all_of(arg.begin(), arg.end(), isHexChar);
// }
/**
 * @brief rmi 命令Run操作的
 * 
 */
void rmiCmd(std::shared_ptr<Command> cmd, vector<string> args){
    logger->set_module("rmi");
    logger->log_info("Start removing images");
    
    //1. 加载镜像仓库
    std::shared_ptr<Store> store;
    store =getStore(cmd);
    auto images=store->image_store;
    logger->log_info("Image store loaded successfully");

    //如果没有输入imageid或者name
    if(args.size()==0){
        logger->log_warning("No image name or ID specified");
        cout<<"image name or ID must be specified"<<endl;
        return;
    }
     //2. 删除指定镜像
    for (const auto& arg : args) {
        logger->log_info("Removing image: " + arg);
        images->Delete(arg);
        logger->log_info("Successfully removed image: " + arg);
    }
    logger->log_info("All specified images removed");

}
