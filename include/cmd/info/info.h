/**
 * @file info.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief info.h 声明与 info 命令相关的结构和方法
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CMD_INFO_INFO_H
#define CMD_INFO_INFO_H
#include <string>
#include <vector>
#include <map>
#include "utils/cli/cobra/command.h"
#include "cmd/root/root.h"
#include <boost/json.hpp>
using std::string;
using std::vector;
/**
 * @brief 用于存储类型信息和相应的数据，支持JSON序列化。
 * 
 */
struct InfoData{
    std::string Type;
    std::map<std::string,std::string> Data;
    friend void tag_invoke(const boost::json::value_from_tag&, boost::json::value& jv, const InfoData& data){
        jv=boost::json::object{
            {data.Type,boost::json::value_from(data.Data)}
        };
    }
    // friend InfoData tag_invoke(const boost::json::value_to_tag<InfoData>, const boost::json::value& jv){
    //     const auto& obj=jv.as_object();
    //     InfoData data;
    //     data.Type=obj.at("Type").as_string().c_str();
    //     data.Data=boost::json::value_to<std::map<std::string,std::string>>(obj.at("Data"));
    //     return data;
    // }
};
void infoCmd(Command& cmd);
std::map<std::string,std::string> hostInfo();
std::map<std::string,std::string> storeInfo(std::shared_ptr<Store> store);
std::vector<InfoData> Information(std::shared_ptr<Store> store);
void init_info();   
#endif