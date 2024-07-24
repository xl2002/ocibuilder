#if !defined(STORAGE_STIRAGE_H)
#define STORAGE_STIRAGE_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "pkg/idtools/idtools.h"
#include "storage/driver/driver.h"
#include "storage/layers.h"
using std::string;
using std::vector;
using std::map;

struct LayerOptions;
class Store{
    public:
    // RunRoot、GraphRoot、GraphDriverName 和 GraphOptions 检索
	//创建对象时传递给 GetStore() 的设置。
    virtual string RunRoot()=0;
    // virtual string GraphRoot()=0;
    // virtual string ImageStore()=0;
	// virtual bool ransientStore()=0;
	// virtual string GraphDriverName()=0;
	// virtual vector<string> GraphOptions()=0;
	// virtual map<string,string> PullOptions()=0;
	// virtual vector<IDMap> UIDMap()=0;
	// virtual vector<IDMap> GIDMap()=0;
    // //GraphDriver 获取并返回所使用的 graph Driver 对象的句柄
	// virtual std::shared_ptr<Driver> GraphDriver()=0;
    //CreateLayer 在底层存储驱动中创建一个新层，
	//可选地具有指定的 ID（如果没有则分配一个）
	//指定），以指定层（或无层）作为其父层，
	//并带有可选名称。  （可写标志被忽略。）
    // virtual std::shared_ptr<Layer> CreateLayer(string id,string parent,vector<string>names,string mountLabel,bool writeable,LayerOptions* options)=0;
    
};

struct LayerOptions{

};

class store:public Store{
    public:
    string run_root;
    string graph_driver_name;
    vector<string> graph_driver_priority;
    
    // shared_ptr<lockfile::LockFile> graph_lock;
    // shared_ptr<lockfile::LockFile> userns_lock;

    string graph_root;
    vector<string> graph_options;
    string image_store_dir;
    map<string, string> pull_options;

    // vector<idtools::IDMap> uid_map;
    // vector<idtools::IDMap> gid_map;

    string auto_userns_user;
    uint32_t auto_ns_min_size;
    uint32_t auto_ns_max_size;

    // rwImageStore image_store;
    // vector<rwImageStore> rw_image_stores;
    // vector<roImageStore> ro_image_stores;
    // rwContainerStore container_store;

    string digest_lock_root;
    bool disable_volatile;
    bool transient_store;

    // lockfile::LastWrite graph_lock_last_write;
    // drivers::Driver graph_driver;
    // rwLayerStore layer_store_use_getters;
    // vector<roLayerStore> ro_layer_stores_use_getters;

    // shared_ptr<idSet> additional_uids;
    // shared_ptr<idSet> additional_gids;
    //注释部分目前还未用到

    public:
    string RunRoot() override;
};

#endif // STORAGE_STIRAGE_H
