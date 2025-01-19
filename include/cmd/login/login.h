/**
 * @file login.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief login.h 声明与 login 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_LOGIN_LOGIN_H
#define CMD_LOGIN_LOGIN_H
#include <string>
#include <vector>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
#include <boost/json.hpp>
using std::string;
using std::vector;
/**
 * @brief LoginOptions 代表 login 标志的结果
 * 
 */
class LoginOptions
{
    public:
    /* data */
    string  username;
    string  password;
    bool    getLogin=false;
};
class user{
    public:
    std::string username;
    std::string password;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const user& image) {
        jv=boost::json::object{{"username",image.username},{"password",image.password}};
    }
    friend user tag_invoke(boost::json::value_to_tag<user>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        user user;
        user.username=obj.at("username").as_string().c_str();
        user.password=obj.at("password").as_string().c_str();
        return user;
    }
};
class Auth{
    public:
    std::map<std::string, user> auth;
    Auth()=default;
    friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Auth& image) {
        jv=boost::json::object{{"auth",boost::json::value_from(image.auth)}};
    }
    friend Auth tag_invoke(boost::json::value_to_tag<Auth>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Auth auth;
        auth.auth=boost::json::value_to<std::map<std::string, user>>(obj.at("auth"));
        return auth;
    }
};

void init_login();      
void loginCmd(Command& cmd, vector<string> args,LoginOptions* iopts);      
#endif