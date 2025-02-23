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
/**
 * @brief 初始化 login 命令的内容
 * 
 */
void init_login(){
    LoginOptions* options=new LoginOptions();
    string name{"login"};
    string Short{"Login to a container registry"};
    string Long{"Login to a container registry on a specified server."};
    string example{"buildah login quay.io"};
    Command* loginCommand=new Command(name,Short,Long,example);
    string Template=UsageTemplate();
    loginCommand->SetUsageTemplate(Template);
    Flagset* flags=loginCommand->Flags();
    flags->StringVar(options->username,"username","","username for the registry");
    flags->StringVar(options->password,"password","","password for the registry");
    flags->BoolVar(options->getLogin,"get-login",false,"get the login command for the registry");
    loginCommand->Run=[=](Command& cmd, vector<string> args){
        loginCmd(cmd,args,options);
    };
    rootcmd.AddCommand({loginCommand});
    // return imagesCommand;
}

/**
 * @brief login 命令Run操作的
 * 
 */
void loginCmd(Command& cmd, vector<string> args,LoginOptions* iopts){
    auto tmp=cmd.flags->actual_flags;
    std::string username=tmp["username"]->value->String();
    std::string password=tmp["password"]->value->String();
    std::string ipAddress = args[args.size()-1];
    user usr;
    usr.username=username;
    usr.password=password;

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
    }
    std::cout << "Credentials saved to auth.json\n";
    // saveLoginInfo(username,password, ipAddress);
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
        return;
    }

    delete iopts;
}