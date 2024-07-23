/**
 * @file Config_Json.h
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 声明与镜像的config文件相关的方法与类
 * @version 0.1
 * @date 2024-07-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H
#include <map>
#include <string>
#include <vector>
using std::map;
using std::string;
using std::vector;
class ContainersConfig{

};

class EngineConfig{

};

class MachineConfig{

};

class NetworkConfig{

};

class SecretConfig{

};

class ConfigMapConfig{

};


class FarmConfig{

};
/**
 * @brief 保存config文件需要的具体信息
 * 
 */
class Configdetails{
    public:
    ContainersConfig*    Containers=nullptr;
    EngineConfig*        Engine=nullptr;
    MachineConfig*       Machine=nullptr;
    NetworkConfig*       Network=nullptr;
    SecretConfig*        Secrets=nullptr;
    ConfigMapConfig*     ConfigMaps=nullptr;
    FarmConfig*          Farms=nullptr;


    void CheckCgroupsAndAdjustConfig();
};
Configdetails ConfigdetailsDefault();
struct DecryptConfig{
    map<string,vector<vector<uint8_t>>>Parameters;
};
#endif