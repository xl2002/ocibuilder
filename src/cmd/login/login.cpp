/**
 * @file login.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对 login.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cmd/login/login.h"
#include "utils/cli/cli/common.h"
#include "storage/types/options.h"
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 初始化 login 命令的内容
 * 
 */
void init_login(){
    auto options=std::make_shared <LoginOptions>();
    string name{"login"};
    string Short{"Login to a container registry"};
    string Long{"Login to a container registry on a specified server."};
    string example{"ocibuilder login quay.io"};
    auto loginCommand=std::make_shared <Command>(name,Short,Long,example);
    string Template=UsageTemplate();
    loginCommand->SetUsageTemplate(Template);
    auto flags=loginCommand->Flags();
    flags->StringVar(options->username,"username","","username for the registry");
    flags->StringVar(options->password,"password","","password for the registry");
    flags->BoolVar(options->getLogin,"get-login",false,"get the login command for the registry");
    loginCommand->Run=[=](std::shared_ptr<Command> cmd, vector<string> args){
        loginCmd(cmd,args,options);
    };
    rootcmd->AddCommand({loginCommand});
    // return imagesCommand;
}

/**
 * @brief login 命令Run操作的
 * 
 */
void loginCmd(std::shared_ptr<Command> cmd, vector<string> args,std::shared_ptr<LoginOptions> iopts){
    logger->set_module("login");
    logger->log_info("Start login command execution");
    
    auto tmp=cmd->flags->actual_flags;
    std::string username=tmp["username"]->value->String();
    std::string password=tmp["password"]->value->String();
    std::string ipAddress = args[args.size()-1];
    
    logger->log_info("Login parameters - username: " + username + ", registry: " + ipAddress);
    user usr;
    usr.username=username;
    usr.password=password;

    // 实现登录命令
    loadLoginInfo(ipAddress);
    std::string host, port;
    auto pos = ipAddress.find(":");
    host = ipAddress.substr(0, pos);
    port = ipAddress.substr(pos + 1);
    login_and_getToken(username, password, host, port, "", "");
    bool flag = login(host, port, username, password);
    if (!flag) {
        std::cerr << "fail to login!!" << "\n";
        logger->log_error("Login failed for username: " + username + " at registry: " + ipAddress);
        return;
    }

    // 如果路径不存在，创建路径
    if (!boost::filesystem::exists("oci_images")) {
        boost::filesystem::create_directories("oci_images");
    }

    
    boost::json::object jsonData = boost::json::object{};
    std::string authPath = "oci_images/auth.json";
    if (boost::filesystem::exists(authPath)) {
        std::ifstream ifs(authPath);
        if (ifs) {
            boost::json::value jv;
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            ifs.close();
            if (!buffer.str().empty()) {
                jv = boost::json::parse(buffer.str());
                if (jv.is_object()) {
                    jsonData = jv.as_object();
                }
            }
    } else {
        std::cerr << "Failed to open file for loading cookie.\n";
        logger->log_error("Failed to open auth file for loading cookie: " + authPath);
        return;
    }
    }
    
    jsonData[ipAddress] = boost::json::value_from(usr);
    std::ofstream ofs(authPath);
    // boost::json::value jv = jsonData;
    std::string formated_info = format_json(boost::json::value(jsonData), 0);
    if (ofs) {
        ofs << formated_info;
        ofs.close();
    } else {
        std::cerr << "Failed to save credentials\n";
        logger->log_error("Failed to save credentials to: " + authPath);
    }
    std::cout << "Credentials saved to auth.json\n";
    logger->log_info("Login successful for username: " + username + " at registry: " + ipAddress);
    // saveLoginInfo(username,password, ipAddress);


    // delete iopts;
}
