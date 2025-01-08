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
#include "filesys/systems.h"
#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <boost/json.hpp>
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
    std::map<std::string, std::string> info;
    // win系统
#ifdef _WIN32
    // 获取主机名
    char hostname[256];
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size) != 0) {
        info["Hostname"] = hostname;
    } else {
        std::cerr << "Failed to get hostname." << std::endl;
    }
    // 获取操作系统版本
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    if (GetVersionEx((OSVERSIONINFO*)&osvi) != 0) {
        std::string osVersion = std::to_string(osvi.dwMajorVersion) + "." +
                                std::to_string(osvi.dwMinorVersion) + "." +
                                std::to_string(osvi.dwBuildNumber);
        info["OS Version"] = osVersion;
    } else {
        std::cerr << "Failed to get OS version." << std::endl;
    }
    // 获取内存信息
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&memInfo) != 0) {
        info["Total RAM (MB)"] = std::to_string(memInfo.ullTotalPhys / (1024 * 1024));
        info["Free RAM (MB)"] = std::to_string(memInfo.ullAvailPhys / (1024 * 1024));
    } else {
        std::cerr << "Failed to get memory information." << std::endl;
    }
#else
    // Linux 下获取主机名
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        info["Hostname"] = hostname;
    } else {
        std::cerr << "Failed to get hostname." << std::endl;
    }

    // Linux 下获取操作系统版本
    struct utsname unameData;
    if (uname(&unameData) == 0) {
        std::string osVersion = std::string(unameData.sysname) + " " +
                                std::string(unameData.release);
        info["OS Version"] = osVersion;
    } else {
        std::cerr << "Failed to get OS version." << std::endl;
    }

    // Linux 下获取内存信息
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        info["Total RAM (MB)"] = std::to_string(memInfo.totalram / (1024 * 1024));
        info["Free RAM (MB)"] = std::to_string(memInfo.freeram / (1024 * 1024));
    } else {
        std::cerr << "Failed to get memory information." << std::endl;
    }
#endif
    return info;
}
/**
 * @brief 获取存储信息
 * 
 * @param store 
 * @return std::map<std::string,std::string> 
 */
std::map<std::string,std::string> storeInfo(std::shared_ptr<Store> store){
    std::map<std::string, std::string> info;
    if (store != nullptr) {
        // 获取运行根目录
        info["Run Root"] = store->RunRoot();
        // 获取图层存储路径
        info["Layer Storage Path"] = store->GetlayerStoragePath();
        // 获取镜像存储路径
        info["Image Storage Path"] = store->GetImageStoragePath();

        auto images=store->image_store->Images();
        info["Image number"] = std::to_string(images.size());
    } else {
        info["Error"] = "Store object is null";
    }
    return info;
}
/**
 * @brief 获取信息
 * 
 * @param store 
 * @return std::vector<InfoData> 
 */

std::vector<InfoData> Information(std::shared_ptr<Store> store){
    std::vector<InfoData> infoDataList;
    // 1. 获取主机信息
    std::map<std::string, std::string> hostInfoMap = hostInfo();
    InfoData hostInfoData;
    hostInfoData.Type = "Host";
    hostInfoData.Data = hostInfoMap;
    infoDataList.push_back(hostInfoData);
    // 2. 获取存储信息
    std::map<std::string, std::string> storeInfoMap = storeInfo(store);
    InfoData storeInfoData;
    storeInfoData.Type = "Store";
    storeInfoData.Data = storeInfoMap;
    infoDataList.push_back(storeInfoData);
    // 3. 返回信息
    return infoDataList;
}
/**
 * @brief info 命令Run操作的
 * 
 */
void infoCmd(Command& cmd){
    //1. 获取镜像库
    std::shared_ptr<Store> store = getStore(&cmd);
    //2. 获取信息
    std::vector<InfoData> infoList = Information(store);
    //3. 格式化打印信息
    boost::json::value jv = boost::json::value_from(infoList);
    std::cout<<format_json(jv)<<std::endl;
}

