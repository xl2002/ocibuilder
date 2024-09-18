#if !defined(STORAGE_STIRAGE_H)
#define STORAGE_STIRAGE_H
#include "pkg/lockfile/lockfile.h"
#include "idtools/idtools.h"
#include "storage/driver/driver.h"
#include "storage/layers.h"
#include "cobra/error.h"
#include "digest/digest.h"
#include "storage/utils.h"
#include <cstdint> 
#include <set>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "idtools/idtools.h"
#include "graphdriver/driver.h"
#include "storage/layers.h"
using std::string;
using std::vector;
using std::map;

struct LayerOptions;
#include <mutex>
#include <filesystem>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;
// 定义 containerLocations 类型
typedef unsigned int containerLocations;
// 定义位标志
constexpr containerLocations stableContainerLocation = 1 << 0; // 1 << 0 => 1
constexpr containerLocations volatileContainerLocation = 1 << 1; // 1 << 1 => 2
struct LayerOptions;
class lockFile;
class Store{
    public:
    // RunRoot、GraphRoot、GraphDriverName 和 GraphOptions 检索
	//创建对象时传递给 GetStore() 的设置。
    virtual string RunRoot()=0;
    virtual void DeleteContainer(std::string id)=0;
    virtual void load()=0;
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
using updateNameOperation = int;
// ImageBigDataOption 结构体定义
struct ImageBigDataOption {
    // Key 是数据项的键
    string key;

    // Data 是要存储的字节数据
    vector<unsigned char> data;

    // Digest 是与数据项关联的摘要
    Digest digest;
};

// ImageOptions 结构体定义
struct ImageOptions {
    // CreationDate 如果不为零，将覆盖默认的创建时间行为
    time_t creationDate;

    // Digest 是用于查找图像的硬编码摘要值， 可选
    Digest digest;

    // Digests 是图像清单的摘要值列表
    vector<Digest> digests;

    // Metadata 是调用者指定的与层关联的元数据
    string metadata;

    // BigData 是与图像一起存储的数据项集合
    vector<ImageBigDataOption> bigData;

    // NamesHistory 用于推测图像创建时的名称，但当前没有任何名称
    vector<string> namesHistory;

    // Flags 是存储在图像中的标志及其值的集合
    map<string, shared_ptr<void>> flags;
};
// Image 结构体定义
struct Image {
    // ID 是创建时指定的 ID 或库生成的随机值
    string id;
    // Digest 是用来定位图像的摘要值（如果创建时指定了）
    Digest digest;
    // Digests 是图像清单的摘要值列表
    vector<Digest> digests;
    // Names 是用户定义的便利值集合，图像可以通过其 ID 或任何名称来引用
    vector<string> names;
    // NamesHistory 是图像过去拥有的名称集合，最新的条目是第一个
    vector<string> namesHistory;
    // TopLayer 是图像本身的顶层 ID（如果图像包含一个或多个层）
    string topLayer;
    // MappedTopLayers 是顶层的替代版本 ID，这些版本具有相同的内容和父层
    vector<string> mappedTopLayers;
    // Metadata 是为调用者提供的便利数据
    string metadata;
    // BigDataNames 是存储的数据显示项名称的列表
    vector<string> bigDataNames;
    // BigDataSizes 映射 BigDataNames 中的名称到已存储数据的大小
    map<string, int64_t> bigDataSizes;
    // BigDataDigests 映射 BigDataNames 中的名称到已存储数据的摘要
    map<string, Digest> bigDataDigests;
    // Created 是图像创建的时间戳
    time_t created;
    // ReadOnly 如果图像位于只读层存储中，则为真
    bool readOnly;
    // Flags 是图像的标志集合
    map<string, shared_ptr<void>> flags;
    void recomputeDigests();
};

class ChildList;
// Define Trie
class Trie {
public:
    std::vector<char> prefix;
    std::shared_ptr<void> item; // Use void* as a placeholder

    int maxPrefixPerNode;
    int maxChildrenPerSparseNode;

    std::shared_ptr<ChildList> children;
};
// Define the ChildList interface
class ChildList {
public:
    virtual ~ChildList() = default;

    virtual size_t length() const = 0;
    virtual std::shared_ptr<Trie> head() const = 0;
    virtual void add(std::shared_ptr<Trie> child) = 0;
    virtual void remove(char byte) = 0;
    virtual void replace(char byte, std::shared_ptr<Trie> child) = 0;
    virtual std::shared_ptr<Trie> next(char byte) const = 0;
    virtual void walk(const std::vector<char>& prefix, void(*visitor)(const std::vector<char>&, const std::shared_ptr<Trie>&)) const = 0;
    virtual void print(std::ostream& os, int indent) const = 0;
    virtual std::shared_ptr<ChildList> clone() const = 0;
    virtual size_t total() const = 0;
};

// Define TruncIndex
class TruncIndex {
public:
    mutable boost::shared_mutex mutex; // Read/Write lock
    std::shared_ptr<Trie> trie;
    std::unordered_map<std::string, bool> ids; // Simple map for illustration

    // Implement additional methods as needed
       // 构造函数，接受一个 std::vector<std::string> 参数并初始化 ids 映射
    TruncIndex(const std::vector<std::string>& idList) {
        // 将 vector 中的每个字符串加入到 ids map 中，默认值设置为 true
        for (const auto& id : idList) {
            ids[id] = true;
        }

        // 初始化 trie 或其他成员变量（如有必要）
        trie = std::make_shared<Trie>();
    }
};
// rwMetadataStore 接口
class rwMetadataStore {
public:
    virtual ~rwMetadataStore() = default;

    // 更新指定 ID 的元数据
    virtual void SetMetadata(const std::string& id, const std::string& metadata) = 0;
};
// rwImageBigDataStore 接口
class rwImageBigDataStore {
public:
    virtual ~rwImageBigDataStore() = default;

    // 存储与 ID 相关的大数据
    virtual void SetBigData(const std::string& id, const std::string& key, 
                            const std::vector<uint8_t>& data, 
                            std::function<std::string(const std::vector<uint8_t>&)> digestManifest) = 0;
};
// flaggableStore 接口
class flaggableStore {
public:
    virtual ~flaggableStore() = default;

    // 移除指定项的标志
    virtual void ClearFlag(const std::string& id, const std::string& flag) = 0;

    // 设置项的标志和值
    virtual void SetFlag(const std::string& id, const std::string& flag, const std::string& value) = 0;
};
// roMetadataStore 接口
class roMetadataStore {
public:
    virtual ~roMetadataStore() = default;

    // //读取与 ID 相关的元数据
    // virtual std::string Metadata(const std::string& id) const = 0;
};
// roBigDataStore 接口
class roBigDataStore {
public:
    virtual ~roBigDataStore() = default;

    // 检索与 ID 和键相关的大数据
    virtual std::vector<uint8_t> BigData(const std::string& id, const std::string& key) const = 0;

    // 检索大数据的大小
    virtual int64_t BigDataSize(const std::string& id, const std::string& key) const = 0;

    // 检索大数据的摘要
    virtual Digest BigDataDigest(const std::string& id, const std::string& key) const = 0;

    // 返回已存储数据的名称列表
    virtual std::vector<std::string> BigDataNames(const std::string& id) const = 0;
};

// roImageStore 接口
class roImageStore : public roMetadataStore, public roBigDataStore {
public:
    virtual ~roImageStore() = default;

    // 开始读取，锁定存储以进行读取
    virtual void startReading() = 0;

    // 释放读取锁
    virtual void stopReading() = 0;

    // 检查是否存在指定 ID 或名称的图像
    virtual bool Exists(const std::string& id) const = 0;

    // 获取指定 ID 或名称的图像信息
    virtual std::shared_ptr<Image> Get(const std::string& id) const = 0;

    // 返回已知图像的列表
    virtual std::vector<Image> Images() const = 0;

    // 根据摘要返回图像列表
    virtual std::vector<std::shared_ptr<Image>> ByDigest(const Digest& digest) const = 0;
};
// rwImageStore 接口
class rwImageStore : public roImageStore, public rwMetadataStore, public rwImageBigDataStore, public flaggableStore {
public:
    virtual ~rwImageStore() = default;

    // 开始写入，锁定存储以进行写入
    virtual void startWriting() = 0;

    // 释放写入锁
    virtual void stopWriting() = 0;

    // 创建具有指定 ID 和可选名称的图像
    virtual std::shared_ptr<Image> create(const std::string& id, const std::vector<std::string>& names, 
                                          const std::string& layer, const ImageOptions& options) = 0;

    // 修改与图像关联的名称
    virtual void updateNames(const std::string& id, const std::vector<std::string>& names, 
                             updateNameOperation op) = 0;

    // 删除图像记录
    virtual void Delete(const std::string& id) = 0;

    // 添加映射的顶部层
    virtual void addMappedTopLayer(const std::string& id, const std::string& layer) = 0;

    // 移除映射的顶部层
    virtual void removeMappedTopLayer(const std::string& id, const std::string& layer) = 0;

    // 清理未引用的每个图像数据
    virtual void GarbageCollect() = 0;

    // 清除所有图像记录
    virtual void Wipe() = 0;
};
struct imageStore:public rwImageStore{
        // rwImageStore 方法的空实现
    void startWriting() override {
        // 空实现
    }

    void stopWriting() override {
        // 空实现
    }

    std::shared_ptr<Image> create(const std::string& id, const std::vector<std::string>& names, 
                                const std::string& layer, const ImageOptions& options) override {
        return nullptr; // 空实现
    }

    void updateNames(const std::string& id, const std::vector<std::string>& names, 
                    updateNameOperation op) override {
        // 空实现
    }

    void Delete(const std::string& id) override {
        // 空实现
    }

    void addMappedTopLayer(const std::string& id, const std::string& layer) override {
        // 空实现
    }

    void removeMappedTopLayer(const std::string& id, const std::string& layer) override {
        // 空实现
    }

    void GarbageCollect() override {
        // 空实现
    }

    void Wipe() override {
        // 空实现
    }

    void startReading() override {
        // 空实现
    }

    void stopReading() override {
        // 空实现
    }

    bool Exists(const std::string& id) const override {
        return false; // 空实现
    }

    std::shared_ptr<Image> Get(const std::string& id) const override {
        return nullptr; // 空实现
    }

    std::vector<Image> Images() const override {
        return {}; // 空实现
    }

    std::vector<std::shared_ptr<Image>> ByDigest(const Digest& digest) const override {
        return {}; // 空实现
    }

    // metadataStore 和 bigDataStore 方法的空实现
    std::vector<uint8_t> BigData(const std::string& id, const std::string& key) const override {
        return {}; // 空实现
    }

    int64_t BigDataSize(const std::string& id, const std::string& key) const override {
        return 0; // 空实现
    }

    Digest BigDataDigest(const std::string& id, const std::string& key) const override {
        return {}; // 空实现
    }

    std::vector<std::string> BigDataNames(const std::string& id) const override {
        return {}; // 空实现
    }

    void SetMetadata(const std::string& id, const std::string& metadata) override {
        // 空实现
    }

    void SetBigData(const std::string& id, const std::string& key, 
                    const std::vector<uint8_t>& data, 
                    std::function<std::string(const std::vector<uint8_t>&)> digestManifest) override {
        // 空实现
    }

    // flaggableStore 方法的空实现
    void ClearFlag(const std::string& id, const std::string& flag) override {
        // 空实现
    }

    void SetFlag(const std::string& id, const std::string& flag, const std::string& value) override {
        // 空实现
    }

    // 以下字段在构建 imageStore 时设置，之后不能再修改。
    // 它们可以在没有其他锁定的情况下安全地访问。
    shared_ptr<lockFile> lockfile; // LockFile 用于区分读写和只读 image store。
    string dir;

    // inProcessLock 只能在持有 lockfile 的情况下获得。
    boost::shared_mutex inProcessLock; // 使用 C++ 的 mutex 代替 Go 的 sync.RWMutex

    // 以下字段只能在持有 inProcessLock 的读写所有权时读取/写入。
    // 几乎所有用户都应使用 startReading() 或 startWriting()。
    lastwrite lastWrite;
    vector<shared_ptr<Image>> images;
    shared_ptr<TruncIndex> idindex;
    //目前没用到
    map<string, shared_ptr<Image>> byid;
    map<string, shared_ptr<Image>> byname;
    map<Digest, vector<shared_ptr<Image>>> bydigest;


    bool checkModified(lastwrite& lastWrite);
    bool load(bool lockedForWriting);
    bool reloadIfChanged(bool lockedForWriting, bool& tryLockedForWriting);
    bool startWritingWithReload(bool canReload);
    std::string imagespath();
    void removeName(std::shared_ptr<Image> image, const std::string& name);
    std::vector<std::string> stringSliceWithoutValue(const std::vector<std::string>& slice, const std::string& value);
    void Save();
};




//metadataStore 接口
class metadataStore : public roMetadataStore, public rwMetadataStore {
public:
    // 实现roMetadataStore接口
};
//containerBigDataStore 接口
class containerBigDataStore : public roBigDataStore {
public:
    virtual ~containerBigDataStore() = default;
    // 定义读写大数据存储的方法
    virtual void SetBigData(const std::string& id, const std::string& key, const std::vector<uint8_t>& data) = 0;
};
class Container {
public:
    // ID 是在创建时指定的 ID 或由库生成的随机值。
    std::string ID;
    // Names 是一组可选的用户定义的便利值。
    std::vector<std::string> Names;
    // ImageID 是用于创建容器的图像的 ID。
    std::string ImageID;
    // LayerID 是容器自身的读写层的 ID。
    std::string LayerID;
    // Metadata 是我们为了方便调用者而保留的数据。
    std::string Metadata;
    // BigDataNames 是我们为了方便调用者而保留的数据项的名称列表。
    std::vector<std::string> BigDataNames;
    // BigDataSizes 将 BigDataNames 中的名称映射到存储数据的大小。
    std::map<std::string, int64_t> BigDataSizes;
    // BigDataDigests 将 BigDataNames 中的名称映射到存储数据的摘要。
    std::map<std::string, Digest> BigDataDigests;
    // Created 是容器创建时的时间戳。
    std::time_t Created; // 使用 std::time_t 表示时间戳
    // UIDMap 和 GIDMap 用于在用户命名空间中设置容器的根文件系统。
    // std::vector<idtools::IDMap> UIDMap;
    // std::vector<idtools::IDMap> GIDMap;
    // Flags 是容器的标志映射。
    map<string, shared_ptr<void>> Flags;
    // volatileStore 如果容器来自临时 JSON 文件，则为 true。
    bool volatileStore;

};
class ContainerBigDataOption {
public:
    // Key 是数据项的键
    std::string key;
    // Data 是与键关联的数据
    std::vector<uint8_t> data;
    // 默认构造函数
    ContainerBigDataOption() = default;
    // 带参数的构造函数
    ContainerBigDataOption(const std::string& key, const std::vector<uint8_t>& data)
        : key(key), data(data) {}

    // 复制构造函数
    ContainerBigDataOption(const ContainerBigDataOption& other) = default;
    // 移动构造函数
    ContainerBigDataOption(ContainerBigDataOption&& other) noexcept = default;
    // 复制赋值运算符
    ContainerBigDataOption& operator=(const ContainerBigDataOption& other) = default;
    // 移动赋值运算符
    ContainerBigDataOption& operator=(ContainerBigDataOption&& other) noexcept = default;
    // 析构函数
    ~ContainerBigDataOption() = default;
};
class ContainerOptions {
public:
    // IDMappingOptions 指定应使用的 ID 映射类型。
    // idmappingoptions::IDMappingOptions idMappingOptions;
    // LabelOpts 是标签选项的列表。
    std::vector<std::string> labelOpts;
    // Flags 是要与容器一起存储的命名标志及其值。
    map<string, shared_ptr<void>> Flags;
    // MountOpts 是挂载选项的列表。
    std::vector<std::string> mountOpts;
    // Volatile 指示容器是否是临时的。
    bool volatileFlag;
    // StorageOpt 是存储选项的映射。
    std::map<std::string, std::string> storageOpt;
    // Metadata 是与容器关联的调用者指定的元数据。
    std::string metadata;
    // BigData 是应为容器存储的一组项。
    std::vector<ContainerBigDataOption> bigData;
};
//rwContainerStore 接口
class rwContainerStore : public metadataStore, public containerBigDataStore, public flaggableStore {
public:
    virtual ~rwContainerStore() = default;
    // 开始写入并锁定存储
    virtual void startWriting() = 0;
    virtual void stopWriting() = 0;
    // 开始读取并锁定存储
    virtual void startReading() = 0;
    virtual void stopReading() = 0;
    // 创建容器
    virtual std::shared_ptr<Container> create(const std::string& id, const std::vector<std::string>& names, 
                                              const std::string& image, const std::string& layer, 
                                              const ContainerOptions& options) = 0;
    // 修改容器的名称
    virtual void updateNames(const std::string& id, const std::vector<std::string>& names, 
                             updateNameOperation op) = 0;
    // 获取容器信息
    virtual std::shared_ptr<Container> Get(const std::string& id) = 0;
    // 检查容器是否存在
    virtual bool Exists(const std::string& id) = 0;
    // 删除容器记录
    virtual void Delete(const std::string& id) = 0;
    // 清除所有容器记录
    virtual void Wipe() = 0;
    // 查找容器的ID
    virtual std::string Lookup(const std::string& name) = 0;
    // 列出所有容器
    virtual std::vector<std::shared_ptr<Container>> Containers() = 0;
    // 清理未引用的数据目录
    virtual void GarbageCollect() = 0;
};
struct containerStore : public rwContainerStore {
public:
    void startWriting() override {
        // 空实现
    }
    void stopWriting() override {
        // 空实现
    }
    void startReading() override {
        // 空实现
    }
    void stopReading() override {
        // 空实现
    }
    std::shared_ptr<Container> create(const std::string& id, const std::vector<std::string>& names, 
                                      const std::string& image, const std::string& layer, 
                                      const ContainerOptions& options) override {
        return nullptr; // 空实现
    }
    void updateNames(const std::string& id, const std::vector<std::string>& names, 
                     updateNameOperation op) override {
        // 空实现
    }
    std::shared_ptr<Container> Get(const std::string& id) override {
        return nullptr; // 空实现
    }
    bool Exists(const std::string& id) override {
        return false; // 空实现
    }
    void Delete(const std::string& id) override {
        // 空实现
    }
    void Wipe() override {
        // 空实现
    }
    std::string Lookup(const std::string& name) override {
        return ""; // 空实现
    }
    std::vector<std::shared_ptr<Container>> Containers() override {
        return {}; // 空实现
    }
    void GarbageCollect() override {
        // 空实现
    }
    // metadataStore 和 bigDataStore 方法的空实现
    void SetMetadata(const std::string& id, const std::string& metadata) override {
        // 空实现
    }
    void SetBigData(const std::string& id, const std::string& key, const std::vector<uint8_t>& data) override {
        // 空实现
    }
    std::vector<uint8_t> BigData(const std::string& id, const std::string& key) const override {
        return {}; // 空实现
    }
    int64_t BigDataSize(const std::string& id, const std::string& key) const override {
        return 0; // 空实现
    }
    Digest BigDataDigest(const std::string& id, const std::string& key) const override {
        return {}; // 空实现
    }
    std::vector<std::string> BigDataNames(const std::string& id) const override {
        return {}; // 空实现
    }
    // flaggableStore 方法的空实现
    void ClearFlag(const std::string& id, const std::string& flag) override {
        // 空实现
    }
    void SetFlag(const std::string& id, const std::string& flag, const std::string& value) override {
        // 空实现
    }
    //其他需要的方法实现
    bool startWritingWithReload(bool canReload);
    bool reloadIfChanged(bool lockedForWriting);
    bool checkModified(lastwrite& lastWrite);
    bool load(bool lockedForWriting);
    void removeName(std::shared_ptr<Container> container, const std::string& name);
    std::vector<std::string> stringSliceWithoutValue(const std::vector<std::string>& slice, const std::string& value);
    containerLocations containerLocation(const std::shared_ptr<Container>& container);
    void Save(containerLocations saveLocations);
public:
    std::shared_ptr<lockFile> lockfile;
    std::string dir;
    std::vector<std::string> jsonPath;

    boost::shared_mutex inProcessLock; // 使用 boost::shared_mutex 替代 sync.RWMutex
    lastwrite lastWrite;
    std::vector<std::shared_ptr<Container>> containers;
    std::shared_ptr<TruncIndex> idindex;
    std::map<std::string, std::shared_ptr<Container>> byid;
    std::map<std::string, std::shared_ptr<Container>> bylayer;
    std::map<std::string, std::shared_ptr<Container>> byname;
};








class store:public Store{
    public:
    string run_root;
    string graph_driver_name;
    vector<string> graph_driver_priority;
    
    shared_ptr<lockFile> graph_lock;
    shared_ptr<lockFile> userns_lock;

    string graph_root;
    vector<string> graph_options;
    string image_store_dir;
    map<string, string> pull_options;

    // vector<idtools::IDMap> uid_map;
    // vector<idtools::IDMap> gid_map;

    string auto_userns_user;
    uint32_t auto_ns_min_size;
    uint32_t auto_ns_max_size;

    shared_ptr<rwImageStore> image_store;
    vector<shared_ptr<rwImageStore>> rw_image_stores;
    vector<shared_ptr<roImageStore>> ro_image_stores;
    shared_ptr<rwContainerStore> container_store;

    string digest_lock_root;
    bool disable_volatile;
    bool transient_store;

    lastwrite graph_lock_last_write;
    shared_ptr<Driver> graph_driver;
    // rwLayerStore layer_store_use_getters;
    // vector<roLayerStore> ro_layer_stores_use_getters;

    // shared_ptr<idSet> additional_uids;
    // shared_ptr<idSet> additional_gids;
    //注释部分目前还未用到

    public:
    string RunRoot() override;
    void load() override;
    shared_ptr<Driver> createGraphDriverLocked();
    shared_ptr<Driver> New(const string& name, const Options& config);
};

#endif //


