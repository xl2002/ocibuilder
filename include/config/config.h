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
#include <unordered_map>
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
struct AufsOptionsConfig {};
struct BtrfsOptionsConfig {};
struct ThinpoolOptionsConfig {};
struct OverlayOptionsConfig {};
struct VfsOptionsConfig {};
struct ZfsOptionsConfig {};
// OptionsConfig structure
struct OptionsConfig {
    std::vector<std::string> AdditionalImageStores;
    std::string ImageStore;
    std::vector<std::string> AdditionalLayerStores;
    std::string Size;
    std::string RemapUIDs;
    std::string RemapGIDs;
    std::string IgnoreChownErrors;
    std::string UseComposefs;
    std::string ForceMask;  // Use string for file mode representation
    std::string RemapUser;
    std::string RemapGroup;
    std::string RootAutoUsernsUser;
    uint32_t AutoUsernsMinSize;
    uint32_t AutoUsernsMaxSize;

    AufsOptionsConfig Aufs;
    BtrfsOptionsConfig Btrfs;
    ThinpoolOptionsConfig Thinpool;
    OverlayOptionsConfig Overlay;
    VfsOptionsConfig Vfs;
    ZfsOptionsConfig Zfs;

    std::string SkipMountHome;
    std::string MountProgram;
    std::string MountOpt;
    std::unordered_map<std::string, std::string> PullOptions;
    bool DisableVolatile;
};

// TomlConfig structure
struct TomlConfig {
    struct Storage {
        std::string Driver;
        std::vector<std::string> DriverPriority;
        std::string RunRoot;
        std::string ImageStore;
        std::string GraphRoot;
        std::string RootlessStoragePath;
        bool TransientStore;
        OptionsConfig Options;
    } Storage;
};
#endif