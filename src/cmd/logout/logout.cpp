/**
 * @file logout.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 logout.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/logout/logout.h"
#include "cmd/login/login.h"
#include "utils/cli/cli/common.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 初始化 logout 命令的内容
 * 
 */
void init_logout(){
    logoutOptions* options=new logoutOptions();
    string name{"logout"};
    string Short{"Logout of a container registry"};
    string Long{"Remove the cached username and password for the registry."};
    string example{"ocibuilder logout quay.io"};
    Command* logoutCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    logoutCommand->SetUsageTemplate(Template);
    Flagset* flags=logoutCommand->Flags();
    flags->SetInterspersed(false);
    flags->BoolVar(options->all,"all",false,"remove all cached credentials");
    logoutCommand->Run=[=](Command& cmd, vector<string> args){
        logoutCmd(cmd,args,options);
    };
    rootcmd.AddCommand({logoutCommand});
    // return imagesCommand;
}

/**
 * @brief logout 命令Run操作的
 * 
 */
void logoutCmd(Command& cmd, vector<string> args, logoutOptions* iopts){
    logger->set_module("logout");
    logger->log_info("Start processing logout command");
    auto tmp=cmd.flags->actual_flags;
    std::string authPath = "oci_images/auth.json";
    // 把auth文件清空
    if (tmp.find("all") != tmp.end()) {
        if (boost::filesystem::exists(authPath)) {
            std::ofstream ofs(authPath);
            ofs << "";
            ofs.close();
            logger->log_info("Cleared all cached credentials");
        } else {
            logger->log_warning("No auth file found when trying to clear all credentials");
        }
        return;
    }
    // 如果没有all参数，则删除指定的用户名和密码
    std::string ipAddr = args[args.size()-1];
    if (!boost::filesystem::exists(authPath)) {
        logger->log_error("No login information found in auth file");
        std::cerr << "No login information\n";
        return;
    }
    std::ifstream ifs(authPath);
    if (!ifs) {
        logger->log_error("Failed to open auth.json for reading");
        std::cerr << "Failed to open auth.json for reading\n";
        return;
    }
    boost::json::value jv;
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    ifs.close();
    jv = boost::json::parse(buffer.str());
    if (!jv.is_object()) {
        logger->log_error("Error parsing JSON from auth file");
        std::cerr << "Error parsing JSON\n";
        return;
    }
    boost::json::object jsonData = jv.as_object();
    if (jsonData.contains(ipAddr)) {
        logger->log_info("Removing login information for " + ipAddr);
        jsonData.erase(ipAddr);
    } else {
        logger->log_warning("No login information found for " + ipAddr);
        std::cerr << "No login information for " << ipAddr << "\n";
        return;
    }
    std::ofstream ofs(authPath);
    std::string formated_info = format_json(boost::json::value(jsonData), 0);
    if (ofs) {
        ofs << formated_info;
        ofs.close();
        logger->log_info("Successfully updated auth file");
    } else {
        logger->log_error("Failed to save credentials to auth file");
        std::cerr << "Failed to save credentials\n";
    }
    delete iopts;
}
