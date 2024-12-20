#include "storage/storage/storage.h"
#include "utils/common/go/time.h"
#include "utils/common/go/string.h"
#include "utils/common/json.h"
#include "utils/common/go/file.h"
#include <iostream>
#include <fstream>
#include <sstream>
namespace fs = boost::filesystem;


// 计算 numContainerLocationIndex
constexpr unsigned int numContainerLocationIndex = 2; // 这里的 2 是因为我们有两个容器位置的标志
string Store:: RunRoot()
{
    return this->run_root;
}
std::string  Store::GetImageStoragePath(){
    return this->run_root+defaultImagestore;
}
std::string Store:: GetlayerStoragePath()
{
    return this->run_root+defaultlayerpath;
}
string join(const vector<string>& elem);
string Join(const vector<string>& elem) {
    return join(elem);
}
shared_ptr<Driver> New(const string& name, const driver_Options& config);


shared_ptr<Driver> Store::createGraphDriverLocked() {
    // driver_Options config{
    //     root: graph_root,
    //     runRoot: run_root,
    //     imageStore: image_store,
    //     driverPriority: graph_driver_priority,
    //     driverOptions: graph_options,
    //     // uidMaps: uidMap,
    //     // gidMaps: gidMap 
    //     experimentalEnabled:false
    // };
    driver_Options config;
    config.root = graph_root;
    config.runRoot = run_root;
    config.imageStore = image_store_dir;
    config.driverPriority = graph_driver_priority;
    config.driverOptions = graph_options;
    auto driver = New(graph_driver_name, config);
    if (!driver) {
        throw myerror("Failed to create graph driver.");
    }
    return driver;
}


bool ImageStore::checkModified(lastwrite& lastWrite) {
    // 从 lockfile 中获取是否修改过的状态以及当前的 lastwrite
    auto ret=lockfile->ModifiedSince(lastWrite);
    auto currentLW = ret.first;
    auto modified = ret.second;
    // auto [currentLW, modified] = lockfile->ModifiedSince(lastWrite);
    
    if (modified) {
        lastWrite = currentLW; // 更新传入的 lastWrite 变量
    }

    return modified; // 返回是否被修改过的状态
}
// imagespath 函数的实现
std::string ImageStore::imagespath(){
    return Join({dir, "index.json"}); // Join 函数用于拼接路径，dir 是 imageStore 的成员变量
}
//parseJson 函数的实现
bool parseJson(const vector<uint8_t>& data, vector<shared_ptr<storage::Image>>& images) {
    try {
        std::string index_str=vectorToString(data);
        storage::index Index=unmarshal<storage::index>(index_str);
        for(auto it:Index.manifests){
            auto image = std::make_shared<storage::Image>();
            image->ID=it.digest;
            // image->digest=std::make_shared<Digest>(it.digest);
            std::string name=it.annotations["org.opencontainers.image.ref.name"];
            std::string newname;
            std::tie(std::ignore,newname,std::ignore)=Cut(name,'/');
            if(newname.size()>0){
                name=newname;
            }
            image->Names.push_back(name);
            image->Digests.emplace_back(it.digest);
            image->image_index=std::make_shared<storage::manifest>(it);
            images.push_back(image);
        }
    } catch (const std::exception& e) {
        throw myerror("Failed to parse JSON: " + std::string(e.what()));
    }
    return true;
}
// 实现 stringSliceWithoutValue 函数
std::vector<std::string> ImageStore::stringSliceWithoutValue(const std::vector<std::string>& slice, const std::string& value) {
    std::vector<std::string> modified;
    modified.reserve(slice.size()); // 预留空间以提高效率
    std::copy_if(slice.begin(), slice.end(), std::back_inserter(modified),
        [&value](const std::string& v) {
            return v != value;
        });
    return modified;
}
// 实现 removeName 函数
void ImageStore::removeName(std::shared_ptr<storage::Image> image, const std::string& name) {
    // 确保持有写锁
    boost::unique_lock<boost::shared_mutex> lock(inProcessLock);
    image->Names = stringSliceWithoutValue(image->Names, name);
}

//将image将 Image 对象转换为 boost::property_tree::ptree 对象
// boost::property_tree::ptree imageToPtree(const std::shared_ptr<storage::Image>& image) {
//     boost::property_tree::ptree pt;
    
//     pt.put("id", image->ID);
//     pt.put("digest", image->digest->digest); // 假设 Digest 结构有一个 `digest` 字段
//     pt.put("topLayer", image->TopLayer);
//     pt.put("metadata", image->Metadata);
//     pt.put("created", time_point_to_string(image->Created)); // 使用 long long 以处理大时间戳

    // Convert vectors to ptree
    // boost::property_tree::ptree digests_ptree;
    // for (const auto& digest : image->Digests) {
    //     boost::property_tree::ptree item;
    //     item.put("", digest.digest); // 假设 Digest 结构有一个 `digest` 字段
    //     digests_ptree.push_back(std::make_pair("", item));
    // }
    // pt.add_child("digests", digests_ptree);

    // boost::property_tree::ptree names_ptree;
    // for (const auto& name : image->Names) {
    //     boost::property_tree::ptree item;
    //     item.put("", name);
    //     names_ptree.push_back(std::make_pair("", item));
    // }
    // pt.add_child("names", names_ptree);

    // Similarly handle namesHistory, mappedTopLayers, bigDataNames
    // For map<string, int64_t> and map<string, Digest>
    // boost::property_tree::ptree bigDataSizes_ptree;
    // for (const auto& pair : image->BigDataSizes) {
    //     boost::property_tree::ptree item;
    //     item.put("", pair.second); // size
    //     bigDataSizes_ptree.add_child(pair.first, item);
    // }
    // pt.add_child("bigDataSizes", bigDataSizes_ptree);

    // boost::property_tree::ptree bigDataDigests_ptree;
    // for (const auto& pair : image->BigDataDigests) {
    //     boost::property_tree::ptree item;
    //     item.put("", pair.second.digest); // digest
    //     bigDataDigests_ptree.add_child(pair.first, item);
    // }
    // pt.add_child("bigDataDigests", bigDataDigests_ptree);

    // // Handle flags (assuming the flag values are serializable to JSON)
    // boost::property_tree::ptree flags_ptree;
    // for (const auto& pair : image->Flags) {
    //     boost::property_tree::ptree item;
    //     // Serialize item based on actual type
    //     // Example below assumes void* is not directly serializable
    //     flags_ptree.add_child(pair.first, item); 
    // }
//     pt.add_child("flags", flags_ptree);

//     pt.put("readOnly", image->ReadOnly);

//     return pt;
// }
//Save 函数的实现
void ImageStore::Save() {
    try {
        // 检查是否允许修改
        if (!lockfile->IsReadWrite()) {
            throw myerror("Not allowed to modify the image store: Store_interface is read-only.");
        }
        lockfile->AssertLockedForWriting();

        std::string rpath = imagespath();
        fs::path dirPath = fs::path(rpath).parent_path(); // 获取目录路径

        // 创建目录
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        // 创建 Boost Property Tree 对象
        // boost::property_tree::ptree ptree_root;
        // for (const auto& image_ptr : images) {
        //     boost::property_tree::ptree image_ptree = imageToPtree(image_ptr);
        //     ptree_root.add_child("images.image", image_ptree);
        // }

        // 写入 JSON 文件
        // std::ofstream ofs(rpath, std::ios::out | std::ios::trunc | std::ios::binary);
        // if (!ofs) {
        //     throw myerror("Failed to open file for writing: " + rpath);
        // }
        
        // boost::property_tree::write_json(ofs, ptree_root);
        // ofs.close();
        // if (!ofs.good()) {
        //     throw myerror("Failed to write data to file.");
        // }
        
        // 记录写入时间
        lastWrite = lockfile->RecordWrite(); // 需要根据实际情况实现

    } catch (const myerror& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}
// load 函数定义
bool ImageStore::load(bool lockedForWriting) {
    try {
        string rpath = imagespath(); // 获取镜像存储路径

        // 使用 Boost 文件系统库打开文件
        boost::filesystem::ifstream file(rpath, std::ios::binary);
        if (!file) {
            if (!boost::filesystem::exists(rpath)) {
                return false; // 文件不存在，无需错误处理
            }
            throw myerror("Failed to read file: " + rpath);
        }

        // 读取镜像文件内容
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if(data.empty()) {
            return false;
        }
        std::vector<std::shared_ptr<storage::Image>> images;
        if (!data.empty()) {
            // 使用 parseJson 函数解析index数据
            if (!parseJson(data, images)) {
                throw myerror("Failed to parse JSON from: " + rpath);
            }
        }

        vector<string> idlist;
        map<string, shared_ptr<storage::Image>> ids;
        map<string, shared_ptr<storage::Image>> names;
        map<Digest, vector<shared_ptr<storage::Image>>> digests;
        std::error_code errorToResolveBySaving;

        // 处理每个镜像
        for (size_t n = 0; n < images.size(); ++n) {
            auto& image = images[n];
            ids[image->ID] = images[n];
            idlist.push_back(image->ID);

            // 检查是否有名称冲突
            for (const auto& name : image->Names) {
                auto it = names.find(name);
                if (it != names.end()) {
                    removeName(it->second, name);
                    errorToResolveBySaving = make_error_code(std::errc::invalid_argument); // ErrDuplicateImageNames
                }
            }

            // 重新计算镜像的摘要
            try {
                image->recomputeDigests(); 
            } catch (const std::exception& e) {
                throw myerror("Computing digests for image with ID " + image->ID + ": " + std::string(e.what()));
            }

            for (const auto& name : image->Names) {
                names[name] = image;
            }

            for (const auto& digest : image->Digests) {
                digests[digest].push_back(image);
            }

            image->ReadOnly = !lockfile->IsReadWrite();
        }

        // 错误处理，如果需要保存镜像信息
        if (errorToResolveBySaving) {
            if (!lockfile->IsReadWrite()) {
                throw myerror("Cannot resolve duplicate image names in read-only mode.");
            }
            if (!lockedForWriting) {
                return true; // 需要写锁才能继续
            }
        }

        // 更新镜像存储信息
        this->images = images;
        idindex = std::make_shared<TruncIndex>(idlist);
        byid = ids;
        byname = names;
        bydigest = digests;

        // 如果需要保存
        if (errorToResolveBySaving) {
            Save(); // 调用保存函数处理
        }

        return true; // 没有错误

    } catch (const myerror& e) {
        // 捕获并重新抛出 myerror 类型的异常
        throw;
    }
}
// reloadIfChanged 函数定义
bool ImageStore::reloadIfChanged(bool lockedForWriting, bool& tryLockedForWriting) {
    // 获取当前的 lastwrite 状态
    lastwrite lastWrite;
    bool modified = false;
    try {
        // 获取 modified 状态
        modified = checkModified(lastWrite);
        if (!modified) {
            return false; // 没有修改，直接返回
        }

        // 获取写锁，确保 `inProcessLock` 是以写模式锁定
        boost::unique_lock<boost::shared_mutex> lock(inProcessLock);
        if (modified) {
            tryLockedForWriting = lockedForWriting;
            if (!load(lockedForWriting)) {
                return tryLockedForWriting; // 加载失败，返回当前尝试状态
            }
            lastWrite = lastWrite; // 更新 lastWrite
        }
    } catch (const std::exception& e) {
        // 处理异常并抛出 myerror 类型的异常
        throw myerror("reloadIfChanged failed: " + std::string(e.what()));
    }

    return false; // 没有错误，未尝试写锁定
}
bool ImageStore::startWritingWithReload(bool canReload) {
    try {
        // 锁定 lockFile 对象
        lockfile->Lock();
        
        // 锁定 inProcessLock
        boost::unique_lock<boost::shared_mutex> inProcessLockGuard(inProcessLock);
        bool succeeded = false;

        // 如果需要重新加载
        if (canReload) {
            bool tryLockedForWriting = false;
            
            // 调用 reloadIfChanged
            bool result = reloadIfChanged(true, tryLockedForWriting);

            // 检查是否有错误并抛出 myerror 类型的异常
            if (!result) {
                throw myerror("Failed to reload if changed");
            }
        }

        // 操作成功
        succeeded = true;
        return succeeded;
    } catch (const std::exception& e) {
        // 处理异常并抛出 myerror 类型的异常
        throw myerror("startWritingWithReload failed: " + std::string(e.what()));
    }
}
shared_ptr<rwImageStore_interface> newImageStore(const string& dir) {
    try {
        // 创建目录
        if (!MkdirAll(dir)) {
            throw myerror("Failed to create directory: " + dir);
        }
        if(!MkdirAll(Join({dir, "blobs/sha256"}))){
            throw myerror("Failed to create directory: " + Join({dir, "blobs"}));
        }
        fs::path indexPath = Join({dir, "index.json"});
        if(!fs::exists(indexPath)){
            fs::ofstream file(indexPath, std::ios::out);
            file.close();
        }
        fs::path layoutpath= Join({dir, "oci_layout"});
        if(!fs::exists(layoutpath)){
            fs::ofstream file(layoutpath, std::ios::out|std::ios::trunc);
            file<<"{\"imageLayoutVersion\": \"1.0.0\"}";
            file.close();
        }
        // 获取锁文件
        auto lockfile = GetLockFile(Join({dir, "images.lock"}));
        if (!lockfile) {
            throw myerror("Failed to get lock file for: " + dir);
        }

        // 初始化 ImageStore 结构
        auto istore = make_shared<ImageStore>();
        // istore->lockfile = lockfile;
        istore->dir = dir;

        // 初始化 ImageStore 的其他成员
        istore->images = vector<shared_ptr<storage::Image>>();
        istore->byid = map<string, shared_ptr<storage::Image>>();
        istore->byname = map<string, shared_ptr<storage::Image>>();
        istore->bydigest = map<Digest, vector<shared_ptr<storage::Image>>>();

        // 开始写入操作
        // if (!istore->startWritingWithReload(false)) {
        //     throw myerror("Failed to start writing with reload.");
        // }

        // 获取最后写入的时间戳
        // istore->lastWrite = istore->lockfile->GetLastWrite();
        // if (istore->lastWrite.state.empty()) {
        //     // throw myerror("Failed to get last write time.");
        // }


        // 加载数据
        if (!istore->load(true)) {
            std::cout<<"the image store is empty, and create a new one"<<std::endl;
        }

        return istore;
    } catch (const myerror& e) {
        throw; // 重新抛出错误以便进一步处理
    }
}




// containerLocationFromIndex 函数定义
containerLocations containerLocationFromIndex(int index) {
    return static_cast<containerLocations>(1 << index);
}
// 将 container 转换为 ptree
// boost::property_tree::ptree containerToPtree(const std::shared_ptr<Container>& container) {
//     boost::property_tree::ptree pt;

//     pt.put("id", container->ID);
//     pt.put("imageID", container->ImageID);
//     pt.put("layerID", container->LayerID);
//     pt.put("metadata", container->Metadata);
//     pt.put("created", static_cast<long long>(container->Created)); // 使用 long long 以处理大时间戳
//     pt.put("volatileStore", container->volatileStore);

    // Convert vector of names to ptree
    // boost::property_tree::ptree names_ptree;
    // for (const auto& name : container->Names) {
    //     boost::property_tree::ptree item;
    //     item.put("", name);
    //     names_ptree.push_back(std::make_pair("", item));
    // }
    // pt.add_child("names", names_ptree);

    // Convert vector of BigDataNames to ptree
    // boost::property_tree::ptree bigDataNames_ptree;
    // for (const auto& name : container->BigDataNames) {
    //     boost::property_tree::ptree item;
    //     item.put("", name);
    //     bigDataNames_ptree.push_back(std::make_pair("", item));
    // }
    // pt.add_child("bigDataNames", bigDataNames_ptree);

    // Convert map of BigDataSizes to ptree
    // boost::property_tree::ptree bigDataSizes_ptree;
    // for (const auto& pair : container->BigDataSizes) {
    //     boost::property_tree::ptree item;
    //     item.put("", pair.second); // size
    //     bigDataSizes_ptree.add_child(pair.first, item);
    // }
    // pt.add_child("bigDataSizes", bigDataSizes_ptree);

    // Convert map of BigDataDigests to ptree
    // boost::property_tree::ptree bigDataDigests_ptree;
    // for (const auto& pair : container->BigDataDigests) {
    //     boost::property_tree::ptree item;
    //     item.put("", pair.second.digest); // digest
    //     bigDataDigests_ptree.add_child(pair.first, item);
    // }
    // pt.add_child("bigDataDigests", bigDataDigests_ptree);

    // Handle flags (assuming the flag values are serializable to JSON)
    // boost::property_tree::ptree flags_ptree;
    // for (const auto& pair : container->Flags) {
    //     boost::property_tree::ptree item;
    //     // Serialize item based on actual type
    //     // Example below assumes void* is not directly serializable
    //     flags_ptree.add_child(pair.first, item); 
    // }
    // pt.add_child("flags", flags_ptree);

//     return pt;
// }
//containerStore::Save函数的实现
void containerStore::Save(containerLocations saveLocations) {
    try {
        // 确保持有写锁
        if (!lockfile->IsReadWrite()) {
            throw myerror("Not allowed to modify the container store: Store_interface is read-only.");
        }
        lockfile->AssertLockedForWriting();

        // 在写文件之前记录写入时间
        lastWrite = lockfile->RecordWrite();
        
        // 遍历所有位置索引
        for (int locationIndex = 0; locationIndex < numContainerLocationIndex; ++locationIndex) {
            containerLocations location = containerLocationFromIndex(locationIndex);
            if ((location & saveLocations) == 0) {
                continue; // 如果不需要保存这个位置，跳过
            }

            std::string rpath = jsonPath[locationIndex];
            fs::path dirPath = fs::path(rpath).parent_path(); // 获取目录路径

            // 创建目录
            if (!fs::exists(dirPath)) {
                fs::create_directories(dirPath);
            }

            // 筛选出符合当前 location 的 containers
            std::vector<std::shared_ptr<Container>> subsetContainers;
            for (const auto& container : containers) {
                if (containerLocation(container) == location) {
                    subsetContainers.push_back(container);
                }
            }

            // 序列化 JSON 数据
            // boost::property_tree::ptree ptree_root;
            // for (const auto& container : subsetContainers) {
            //     boost::property_tree::ptree container_ptree = containerToPtree(container);
            //     ptree_root.add_child("containers.container", container_ptree);
            // }

            // 写入 JSON 文件
            // std::ofstream ofs(rpath, std::ios::out | std::ios::trunc | std::ios::binary);
            // if (!ofs) {
            //     throw myerror("Failed to open file for writing: " + rpath);
            // }
            
            // boost::property_tree::write_json(ofs, ptree_root);
            // ofs.close();
            // if (!ofs.good()) {
            //     throw myerror("Failed to write data to file: " + rpath);
            // }

            // 处理 volatileContainerLocation 的特殊情况（如需要）
            // if (location == volatileContainerLocation) {
            //     // 可以添加额外的选项或逻辑处理，如在写入文件时禁用同步等
            //     // 示例：禁用同步选项的逻辑
            // }
        }

    } catch (const myerror& ex) {
        throw;
    }
}

void containerStore::startWriting() {
    // 空实现
}
void containerStore::stopWriting() {
    // 空实现
}
void containerStore::startReading() {
    // 空实现
}
void containerStore::stopReading() {
// 空实现
}
std::vector<std::string> dedupeStrings(const std::vector<std::string>& names);
/**
 * @brief 创建container信息
 * 
 * @param id 
 * @param names 
 * @param image 
 * @param layer 
 * @param options 
 * @return std::shared_ptr<Container> 
 */
std::shared_ptr<Container> containerStore::create(const std::string& id, const std::vector<std::string>& names, 
                                                  const std::string& image, const std::string& layer, 
                                                  const ContainerOptions& options) {
    try {
        // Step 1: 初始化 ContainerOptions
        ContainerOptions containerOptions = options;

        // Step 2: 确保 ID 唯一
        std::string containerID = id.empty() ? GenerateRandomID() : id;
        while (byid.find(containerID) != byid.end()) {
            containerID = GenerateRandomID();
        }

        // Step 3: 检查名称唯一性
        auto uniqueNames = dedupeStrings(names);
        for (const auto& name : uniqueNames) {
            if (byname.find(name) != byname.end()) {
                throw std::runtime_error("Container name '" + name + "' is already in use.");
            }
        }

        // Step 4: 初始化 Container 对象
        auto container = std::make_shared<Container>();
        container->ID = containerID;
        container->Names = uniqueNames;
        container->ImageID = image;
        container->LayerID = layer;
        container->Metadata = containerOptions.metadata;
        container->Flags = containerOptions.Flags;
        container->BigDataNames = {};
        container->BigDataSizes = {};
        container->Created = std::time(nullptr);
        container->volatileStore = containerOptions.volatileFlag;

        // Step 5: 更新内部存储
        byid[containerID] = container;
        bylayer[layer] = container;
        for (const auto& name : uniqueNames) {
            byname[name] = container;
        }

        return container;
    } catch (const std::exception& ex) {
        throw myerror(std::string("Failed to create container: ") + ex.what());
    }
}
void containerStore::updateNames(const std::string& id, const std::vector<std::string>& names, 
                    updateNameOperation op) {
    // 空实现
}
std::shared_ptr<Container> containerStore::Get(const std::string& id) {
    return nullptr; // 空实现
}
bool containerStore::Exists(const std::string& id) {
    return false; // 空实现
}
void containerStore::Delete(const std::string& id) {
    // 空实现
}
void containerStore::Wipe() {
    // 空实现
}
std::string containerStore::Lookup(const std::string& name) {
    return ""; // 空实现
}
std::vector<std::shared_ptr<Container>> containerStore::Containers() {
    return {}; // 空实现
}
void containerStore::GarbageCollect() {
    // 空实现
}
// metadataStore 和 bigDataStore 方法的空实现
void containerStore::SetMetadata(const std::string& id, const std::string& metadata) {
    // 空实现
}
void containerStore::SetBigData(const std::string& id, const std::string& key, const std::vector<uint8_t>& data) {
    // 空实现
}
std::vector<uint8_t> containerStore::BigData(const std::string& id, const std::string& key)  {
    return {}; // 空实现
}
int64_t containerStore::BigDataSize(const std::string& id, const std::string& key)  {
    return 0; // 空实现
}
Digest containerStore::BigDataDigest(const std::string& id, const std::string& key)  {
    return {}; // 空实现
}
std::vector<std::string> containerStore::BigDataNames(const std::string& id)  {
    return {}; // 空实现
}
// flaggableStore_interface 方法的空实现
void containerStore::ClearFlag(const std::string& id, const std::string& flag) {
    // 空实现
}
void containerStore::SetFlag(const std::string& id, const std::string& flag, const std::string& value) {
    // 空实现
}

//containerStore::containerLocation函数的实现
containerLocations containerStore::containerLocation(const std::shared_ptr<Container>& container) {
    // 确保持有读锁
    boost::shared_lock<boost::shared_mutex> lock(inProcessLock);

    if (container->volatileStore) {
        return volatileContainerLocation;
    }
    return stableContainerLocation;
}
//containerStore::stringSliceWithoutValue函数的实现
std::vector<std::string> containerStore::stringSliceWithoutValue(const std::vector<std::string>& slice, const std::string& value) {
    std::vector<std::string> modified;
    modified.reserve(slice.size()); // 预留空间以提高效率

    std::copy_if(slice.begin(), slice.end(), std::back_inserter(modified),
        [&value](const std::string& v) {
            return v != value;
        });

    return modified;
}
//containerStore::removeName函数的实现
void containerStore::removeName(std::shared_ptr<Container> container, const std::string& name) {
    // 确保持有写锁
    boost::unique_lock<boost::shared_mutex> lock(inProcessLock);

    // 调用 stringSliceWithoutValue 函数从 container 的 Names 列表中删除 name
    container->Names = stringSliceWithoutValue(container->Names, name);
}

bool parseJsonContainers(const std::vector<uint8_t>& data, std::vector<std::shared_ptr<Container>>& containers) {
    try {
        // 将二进制数据转换为字符串
        // std::string json_str(data.begin(), data.end());
        
        // // 使用字符串流读取 JSON
        // std::stringstream ss(json_str);

        // // 解析 JSON 数据
        // ptree root;
        // read_json(ss, root);

        // for (const auto& item : root) {
        //     auto container = std::make_shared<Container>();
        //     container->ID = item.second.get<std::string>("id");

        //     // 解析名称
        //     for (const auto& name : item.second.get_child("names")) {
        //         container->Names.push_back(name.second.data());
        //     }

        //     // 解析 LayerID
        //     container->LayerID = item.second.get<std::string>("layerID");

        //     containers.push_back(container);
        // }
    } catch (const std::exception& e) {
        throw myerror("Failed to parse JSON: " + std::string(e.what()));
    }
    return true;
}
bool containerStore::load(bool lockedForWriting) {
    try {
        // 定义并初始化容器
        vector<shared_ptr<Container>> containers;
        map<string, shared_ptr<Container>> ids;
        map<string, shared_ptr<Container>> layers;
        map<string, shared_ptr<Container>> names;
        containerLocations modifiedLocations = 0;
        std::error_code errorToResolveBySaving;

        for (int locationIndex = 0; locationIndex < numContainerLocationIndex; ++locationIndex) {
            auto location = containerLocationFromIndex(locationIndex);
            auto rpath = jsonPath[locationIndex];
            
            // 使用 Boost 文件系统库读取文件内容
            boost::filesystem::ifstream file(rpath, std::ios::binary);
            if (!file) {
                if (!boost::filesystem::exists(rpath)) {
                    continue; // 文件不存在，无需错误处理
                }
                throw myerror("Failed to read file: " + rpath);
            }

            std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            std::vector<shared_ptr<Container>> locationContainers;
            if (!data.empty()) {
                // 使用 parseJson 函数解析数据
                if (!parseJsonContainers(data, locationContainers)) {
                    throw myerror("Failed to parse JSON from: " + rpath);
                }
            }

            // 处理每个容器
            for (const auto& container : locationContainers) {
                // 检查是否有重复 ID
                if (ids.find(container->ID) != ids.end()) {
                    continue; // 跳过重复容器
                }

                // 记住容器来源位置
                if (location == volatileContainerLocation) {
                    container->volatileStore = true;
                }
                containers.push_back(container);
                ids[container->ID] = container;
            }
        }

        vector<string> idlist;
        for (const auto& container : containers) {
            idlist.push_back(container->ID);
            layers[container->LayerID] = container;
            for (const auto& name : container->Names) {
                auto it = names.find(name);
                if (it != names.end()) {
                    removeName(it->second, name);
                    errorToResolveBySaving = make_error_code(std::errc::invalid_argument); // ErrDuplicateContainerNames
                    
                    // 调用 containerLocation 方法，并更新 modifiedLocations
                    modifiedLocations |= containerLocation(container);
                }
                names[name] = container;
            }
        }


        // 更新容器存储信息
        this->containers = containers;
        this->idindex = std::make_shared<TruncIndex>(idlist);
        this->byid = ids;
        this->bylayer = layers;
        this->byname = names;

        // 错误处理，处理可能需要保存的数据
        if (errorToResolveBySaving) {
            if (!lockedForWriting) {
                return true; // 需要写锁才能继续
            }
            Save(modifiedLocations);
        }
        return true; // 没有错误

    } catch (const myerror& e) {
        // 捕获并重新抛出 myerror 类型的异常
        throw;
    }
}
bool containerStore::checkModified(lastwrite& lastWrite) {
    // 从 lockfile 中获取是否修改过的状态以及当前的 lastwrite
    auto ret=lockfile->ModifiedSince(lastWrite);
    auto currentLW = ret.first;
    auto modified = ret.second;
    // auto [currentLW, modified] = lockfile->ModifiedSince(lastWrite);

    if (modified) {
        lastWrite = currentLW; // 更新传入的 lastWrite 变量
    }

    return modified; // 返回是否被修改过的状态
}
/// @brief containerStore::reloadIfChanged函数的实现
/// @param lockedForWriting 
/// @return 
bool containerStore::reloadIfChanged(bool lockedForWriting) {
    lastwrite lastWrite;
    bool modified = false;
    bool tryLockedForWriting = false;
    
    try {
        // 获取 modified 状态
        modified = checkModified(lastWrite);

        // 如果有修改
        if (modified) {
            // 获取写锁，确保 `inProcessLock` 是以写模式锁定
            boost::unique_lock<boost::shared_mutex> lock(inProcessLock);

            // 尝试加载数据
            tryLockedForWriting = lockedForWriting;
            if (!load(lockedForWriting)) {
                return tryLockedForWriting; // 加载失败，返回当前尝试状态
            }

            // 更新 lastWrite
            this->lastWrite = lastWrite;
        }
    } catch (const std::exception& e) {
        // 处理异常并抛出 myerror 类型的异常
        throw myerror("reloadIfChanged failed: " + std::string(e.what()));
    }

    return false; // 没有错误，未尝试写锁定
}
/// @brief containerStore::startWritingWithReload函数实现
/// @param canReload 
/// @return 
bool containerStore::startWritingWithReload(bool canReload) {
    try {
        // 锁定 lockFile 对象
        lockfile->Lock();

        // 锁定 inProcessLock
        boost::unique_lock<boost::shared_mutex> inProcessLockGuard(inProcessLock);
        
        if (canReload) {
            // 调用 reloadIfChanged
            bool tryLockedForWriting = false;
            if (!reloadIfChanged(true)) {
                throw myerror("Failed to reload if changed.");
            }
        }

        // 操作成功
        return true;
    } catch (const std::exception& e) {
        // 处理异常并抛出 myerror 类型的异常
        throw myerror("startWritingWithReload failed: " + std::string(e.what()));
    }
}
//newContainerStore的实现
std::shared_ptr<rwContainerStore_interface> newContainerStore(const std::string& dir, const std::string& runDir, bool transient) {
    try {
        // 创建目录
        if (!MkdirAll(dir)) {
            throw myerror("Failed to create directory: " + dir);
        }

        std::string volatileDir = dir;
        if (transient) {
            if (!MkdirAll(runDir)) {
                throw myerror("Failed to create transient directory: " + runDir);
            }
            volatileDir = runDir;
        }
        // 获取锁文件
        auto lockfile = GetLockFile(Join({volatileDir, "containers.lock"}));
        if (!lockfile) {
            throw myerror("Failed to get lock file for: " + volatileDir);
        }

        // 初始化 containerStore 结构
        auto cstore = std::make_shared<containerStore>();
        cstore->lockfile = lockfile;
        cstore->dir = dir;
        // 确保 jsonPath 至少有一个元素
        cstore->jsonPath.push_back(Join({dir, "containers.json"}));
        // 确保 jsonPath 至少有一个元素
        cstore->jsonPath.push_back(Join({volatileDir, "containers.json"}));
        // cstore->jsonPath[0] = Join({dir, "containers.json"});
        // cstore->jsonPath[1] = Join({volatileDir, "volatile-containers.json"});

        // 初始化 containerStore 的其他成员
        cstore->containers = std::vector<std::shared_ptr<Container>>();
        cstore->byid = std::map<std::string, std::shared_ptr<Container>>();
        cstore->bylayer = std::map<std::string, std::shared_ptr<Container>>();
        cstore->byname = std::map<std::string, std::shared_ptr<Container>>();

        // 开始写入操作
        if (!cstore->startWritingWithReload(false)) {
            throw myerror("Failed to start writing with reload.");
        }

        // 获取最后写入的时间戳
        cstore->lastWrite = cstore->lockfile->GetLastWrite();
        if (cstore->lastWrite.state.empty()) {
            // throw myerror("Failed to get last write time.");
        }

        // 加载数据
        if (!cstore->load(true)) {
            throw myerror("Failed to load container store.");
        }

        return cstore;
    } catch (const myerror& e) {
        throw; // 重新抛出错误以便进一步处理
    }
}






void load(Store* s) {
    try {
        // Lambda 捕获 store 指针
        auto func = [s]() -> std::shared_ptr<Driver> {
            s->graph_lock->Lock();
            // 获取上次写入的值
            lastwrite lastWrite = s->graph_lock->GetLastWrite(); // 获取上次写入的内容，以字节形式呈现
            s->graph_lock_last_write = lastWrite;

            // 创建 Driver
            auto driver = s->createGraphDriverLocked(); // 创建 Driver
            if (!driver) {
                throw myerror("Failed to create graph driver.");
            } 
            s->graph_driver = driver;
            s->graph_driver_name = driver->String(); // 这里假设 Driver 有一个 String 方法
            s->graph_lock->Unlock();
            return driver;
        }();

        // 如果函数返回的结果为空，抛出错误
        if (!func) {
            throw myerror("Function returned null driver.");
        }

        // 获取 driverPrefix
        string driverPrefix = "oci_";

        // 获取 imgStoreRoot 路径
        string imgStoreRoot = s->image_store_dir;
        
        if (imgStoreRoot.empty()) {
            imgStoreRoot = s->graph_root+"\\"+driverPrefix + "registry";
        }

        // 创建图像存储目录
        string gipath=imgStoreRoot;
        if (!MkdirAll(gipath)) {
            throw myerror("Failed to create directory: " + gipath);
        }

        // 创建 ImageStore,即oci_registry文件夹
        shared_ptr<rwImageStore_interface> imageStore = newImageStore(gipath);
        if (!imageStore) {
            throw myerror("Failed to create ImageStore at: " + gipath);
        }
        s->image_store = imageStore;
        s->rw_image_stores.push_back(imageStore);

        // 创建容器存储目录
        string gcpath = Join({s->graph_root, driverPrefix + "containers"});
        if (!MkdirAll(gcpath)) {
            throw myerror("Failed to create directory: " + gcpath);
        }
        string rcpath = Join({s->run_root, driverPrefix + "containers"});
        if (!MkdirAll(rcpath)) {
            throw myerror("Failed to create directory: " + rcpath);
        }
        // 创建 ContainerStore
        shared_ptr<rwContainerStore_interface> rcs = newContainerStore(gcpath, rcpath, s->transient_store);
        if (!rcs) {
            throw myerror("Failed to create ContainerStore.");

        }
        s->container_store = rcs;

        // 处理额外的 ImageStores
        vector<string> additionalImageStores = s->graph_driver->AdditionalImageStores();
        if (!s->image_store_dir.empty()) {
            additionalImageStores.insert(additionalImageStores.begin(), s->graph_root);
        }

        // for (const string& store : additionalImageStores) {
        //     string gipath = Join({store, driverPrefix + "images"});
        //     shared_ptr<roImageStore> ris;

        //     if (store == s->image_store || store == s->graph_root) {
        //         imageStore = newImageStore(gipath);
        //         if (!imageStore) {
        //             throw myerror("Failed to create ImageStore at: " + gipath);
        //         }
        //         s->rw_image_stores.push_back(imageStore);
        //         ris = imageStore;
        //     } else {
        //         ris = newROImageStore(gipath);
        //         if (!ris) {
        //             if (errno == EROFS) {
        //                 // 如果是只读文件系统，跳过锁文件创建
        //                 // logDebug("Ignoring creation of lockfiles on read-only file systems: " + gipath);
        //                 continue;
        //             }
        //             throw myerror("Failed to create ROImageStore at: " + gipath);
        //         }
        //     }
        //     s->ro_image_stores.push_back(ris);
        // }

        // 创建锁文件目录
        // s->digest_lock_root = Join({s->run_root, driverPrefix + "locks"});
        // if (!MkdirAll(s->digest_lock_root)) {
        //     throw myerror("Failed to create directory: " + s->digest_lock_root);
        // }

    } catch (const myerror& e) {
        throw; // 重新抛出错误以便进一步处理
    }
}

void Store::load() {
    try {
        ::load(this);
    } catch (const myerror& e) {
        throw; // 重新抛出 myerror 类型的异常
    }
}
void Store::DeleteContainer(std::string id){
    return;
}
std::shared_ptr<storage::Image> Store::Image(std::string id){
    std::vector<std::shared_ptr<roImageStore_interface>> images_store;
    images_store.push_back(this->image_store);
    if(this->ro_image_stores.size()>0){
        images_store.insert(images_store.end(), this->ro_image_stores.begin(), this->ro_image_stores.end());
    }
    for(auto store:images_store){
        auto img=store->Get(id);
        if(img!=nullptr){
            return img;
        }
    }
    return nullptr;
}

std::shared_ptr<storage::Image> ImageStore::Get(const std::string& id) {
    auto image=this->lookup(id);
    if(image!=nullptr){
        std::string manifestpath=this->dir+"/blobs/sha256/"+image->digest->Encoded();
        std::ifstream manifestf(manifestpath,std::ios::binary);
        std::ostringstream oss;
        oss << manifestf.rdbuf();
        auto manifest=unmarshal<Manifest>(oss.str());
        image->image_manifest=std::make_shared<Manifest>(manifest);
        manifestf.close();

        std::string configfile=this->dir+"/blobs/sha256/"+manifest.Config.Digests.Encoded();
        std::ifstream configf(configfile,std::ios::binary);
        std::ostringstream oss2;
        oss2 << configf.rdbuf();
        auto config=unmarshal<v1::Image>(oss2.str());
        image->image_config=std::make_shared<v1::Image>(config);
        configf.close();
        return std::make_shared<storage::Image>(*image);//复制一份
    }
    return nullptr;
}

std::shared_ptr<storage::Image> ImageStore::lookup(const std::string& id){
    auto image=this->byid.find(id);
    if(image!=this->byid.end()){
        return image->second;
    }
    auto img2=this->byname.find(id);
    if(img2!=this->byname.end()){
        return img2->second;
    }
    return nullptr;
}
void ImageStore::startWriting() {
    // 空实现
}

void ImageStore::stopWriting(){
    // 空实现
}

std::shared_ptr<storage::Image> ImageStore::create(
    const std::string& id, const std::vector<std::string>& names, 
    const std::string& layer, const ImageOptions& options) {
    return nullptr; // 空实现
}

void ImageStore::updateNames(const std::string& id, const std::vector<std::string>& names, 
                updateNameOperation op) {
    // 空实现
}

void ImageStore::Delete(const std::string& id) {
    // 空实现
}

void ImageStore::addMappedTopLayer(const std::string& id, const std::string& layer) {
    // 空实现
}

void ImageStore::removeMappedTopLayer(const std::string& id, const std::string& layer) {
    // 空实现
}

void ImageStore::GarbageCollect() {
    // 空实现
}

void ImageStore::Wipe() {
    // 空实现
}

void ImageStore::startReading() {
    // 空实现
}

void ImageStore::stopReading() {
    // 空实现
}

bool ImageStore::Exists(const std::string& id)  {
    return false; // 空实现
}

// std::shared_ptr<storage::Image> Get(const std::string& id)  {
//     return nullptr; // 空实现
// }

std::vector<storage::Image> ImageStore::Images()  {
    return {}; // 空实现
}

std::vector<std::shared_ptr<storage::Image>> ImageStore::ByDigest(const Digest& digest)  {
    return {}; // 空实现
}

// metadataStore 和 bigDataStore 方法的空实现
std::vector<uint8_t> ImageStore::BigData(const std::string& id, const std::string& key) {
    return {}; // 空实现
}

int64_t ImageStore::BigDataSize(const std::string& id, const std::string& key) {
    return 0; // 空实现
}

Digest ImageStore::BigDataDigest(const std::string& id, const std::string& key) {
    return {}; // 空实现
}

std::vector<std::string> ImageStore::BigDataNames(const std::string& id) {
    return {}; // 空实现
}

void ImageStore::SetMetadata(const std::string& id, const std::string& metadata) {
    // 空实现
}

void ImageStore::SetBigData(const std::string& id, const std::string& key, 
                const std::vector<uint8_t>& data, 
                std::function<std::string(const std::vector<uint8_t>&)> digestManifest) {
    // 空实现
}

// flaggableStore_interface 方法的空实现
void ImageStore::ClearFlag(const std::string& id, const std::string& flag) {
    // 空实现
}

void ImageStore::SetFlag(const std::string& id, const std::string& flag, const std::string& value) {
    // 空实现
}
/**
 * @brief 得到overlay中layer
 * 
 * @return std::shared_ptr<rwLayerStore_interface> 
 */
std::shared_ptr<rwLayerStore_interface> Store::bothLayerStoreKinds() {
    try {
        // 只需要返回rwLayerStore
        return bothLayerStoreKindsLocked();
    } catch (const myerror& e) {
        throw myerror("Error in bothLayerStoreKinds: ");  // 重新抛出myerror
    }
}

/**
 * @brief 得到overlay中layer
 * 
 * @return std::shared_ptr<rwLayerStore_interface> 
 */
std::shared_ptr<rwLayerStore_interface> Store::bothLayerStoreKindsLocked() {
    // 获取 primary layer store
    auto primary = getLayerStoreLocked();
    if (!primary) {
        throw myerror("Error loading primary layer store");
    }
    
    // 这里只需要返回 primary，不需要处理只读层
    return primary;
}

/**
 * @brief 返回overly的内容
 * 
 * @return std::shared_ptr<rwLayerStore_interface> 
 */
std::shared_ptr<rwLayerStore_interface> Store::getLayerStoreLocked() {
    try {
        if(this->layer_store_use_getters!=nullptr){
            return this->layer_store_use_getters;
        }
        // 检查路径是否存在，模拟 Go 中的路径处理
        std::string driverPrefix = this->graph_driver_name;
        
        // 假设 s.runRoot 是 Store 的成员变量
        boost::filesystem::path rlpath = boost::filesystem::path(this->run_root) / (driverPrefix );
        boost::filesystem::path glpath = boost::filesystem::path(this->graph_root) / (driverPrefix );


        // 创建路径
        if (!boost::filesystem::exists(rlpath)) {
            boost::filesystem::create_directories(rlpath);
        }
        if (!boost::filesystem::exists(glpath)) {
            boost::filesystem::create_directories(glpath);
        }

        // 检查 imageStoreDir 是否存在
        std::string ilpath = "";
        // if (!this->imageStoreDir.empty()) {
        //     ilpath = this->imageStoreDir + "/" + driverPrefix + "layers";
        // }
        std::cout<<"this->newLayerStore"<<std::endl;std::cout<<this->graph_driver->imageStore<<std::endl;
        // 调用 newLayerStore 函数
        auto rls = this->newLayerStore(rlpath.string(), glpath.string(), ilpath, this->graph_driver, this->transient_store);
        if (!rls) {
            throw myerror("Failed to create layer store");
        }

        this->layer_store_use_getters = rls;
        return rls;
    } catch (const myerror& e) {
        std::cerr << "Error in getLayerStoreLocked: " << e.what() << std::endl;
        throw;
    }
}
/**
 * @brief 读取overlay中的layer
 * 
 * @param rundir 
 * @param layerdir 
 * @param imagedir 
 * @param driver 
 * @param transient 
 * @return std::shared_ptr<rwLayerStore_interface> 
 */
std::shared_ptr<rwLayerStore_interface> Store::newLayerStore(
    std::string rundir, 
    std::string layerdir, 
    std::string imagedir,
    std::shared_ptr<Driver> driver,
    bool transient) {

    try {
        // 检查路径是否有效并创建目录
        if (!boost::filesystem::exists(rundir)) {
            if (!boost::filesystem::create_directories(rundir)) {
                throw myerror("Failed to create rundir: " + rundir);
            }
        }
        if (!boost::filesystem::exists(layerdir)) {
            if (!boost::filesystem::create_directories(layerdir)) {
                throw myerror("Failed to create layerdir: " + layerdir);
            }
        }
        if (!imagedir.empty() && !boost::filesystem::exists(imagedir)) {
            if (!boost::filesystem::create_directories(imagedir)) {
                throw myerror("Failed to create imagedir: " + imagedir);
            }
        }std::cout<<"rundir: "<<rundir<<"layerdir: "<<layerdir<<"imagedir: "<<imagedir<<std::endl;
        std::string layerfile=layerdir+"/"+"layers.json";
        if(!boost::filesystem::exists(layerfile)){
            boost::filesystem::path file(layerfile);
            boost::filesystem::ofstream layers(file,std::ios::out);
            if(!layers.is_open()){
                throw myerror("Failed to create layerfile: "+layerfile);
            }
            layers.close();
        }
        // 使用 std::make_shared 创建 rwLayerStore_interface 对象
        auto layerstore = std::make_shared<layerStore>();
        
        // 逐个赋值初始化成员变量
        layerstore->rundir = rundir;
        layerstore->layerdir = layerdir;
        layerstore->driver = driver;
                // 初始化映射
        layerstore->byid = std::map<std::string, std::shared_ptr<Layer>>();
        layerstore->byname = std::map<std::string, std::shared_ptr<Layer>>();
        layerstore->bymount = std::map<std::string,std::shared_ptr<Layer>>();
        
        std::string volatileDir = transient ? rundir : layerdir; // 如果 transient 为 true，使用 rundir 作为 volatileDir
        layerstore->jsonPath[0] = (boost::filesystem::path(layerdir) / "layers.json").string();
        // layerstore->jsonPath[1] = (boost::filesystem::path(volatileDir) / "volatile-layers.json").string();
        if(isDirectoryEmpty(layerstore->rundir)){
            return layerstore;
        }
        // 调用 load 方法读取 overlay 内容
        if (!layerstore->load(true)) {
            throw myerror("Failed to load overlay content.");
        }

        return layerstore;
    } catch (const myerror& ex) {
        std::cerr << "Layer store initialization error: " << ex.what() << std::endl;
        return nullptr;
    }
}
/**
 * @brief 在overlay中创建copy的layer，新建container的信息，并不创建文件夹
 * 
 * @param id 
 * @param names 
 * @param image 
 * @param layer 
 * @param metadata 
 * @param options 
 * @return std::shared_ptr<Container> 
 */
std::shared_ptr<Container> Store::CreateContainer(
    const std::string& id, 
    const std::vector<std::string>& names, 
    const std::string& image, 
    const std::string& layer, 
    const std::string& metadata, 
    const std::shared_ptr<ContainerOptions> options) 
{
    try {
        // Step 1: 准备 layerOptions
        auto layerOptions = std::make_shared<LayerOptions>();
        
        layerOptions->idMappingOptions->HostUIDMapping = options->idMappingOptions->HostUIDMapping;
        layerOptions->idMappingOptions->HostGIDMapping = options->idMappingOptions->HostGIDMapping;
        layerOptions->idMappingOptions->UIDMap = options->idMappingOptions->UIDMap; // 复制 UIDMap
        layerOptions->idMappingOptions->GIDMap = options->idMappingOptions->GIDMap; // 复制 GIDMap

        // Step 2: 创建 Layer
        auto rlstore = this->bothLayerStoreKinds();
        std::ifstream diff;
        std::shared_ptr<Layer> clayer;
        std::string layerName = layer;
        std::tie(clayer, std::ignore) = rlstore->create(
            layerName, nullptr, {}, "", {}, layerOptions, true, diff);

        // Step 3: 准备 Container 元信息
        auto containerOptions = std::make_shared<ContainerOptions>();
        containerOptions->idMappingOptions->HostUIDMapping = options->idMappingOptions->UIDMap.empty();
        containerOptions->idMappingOptions->HostGIDMapping = options->idMappingOptions->GIDMap.empty();
        containerOptions->idMappingOptions->UIDMap = options->idMappingOptions->UIDMap; // 复制 UIDMap
        containerOptions->idMappingOptions->GIDMap = options->idMappingOptions->GIDMap; // 复制 GIDMap

        // Step 4: 创建 Container
        auto container = this->container_store->create(
            id, 
            names, 
            image, 
            clayer->ID, 
            *containerOptions);

        return container;

    } catch (const myerror& e) {
        std::cerr << "Error while creating container: " << e.what() << std::endl;
        throw;
    }
}

bool Store::savelayers(){
    return this->layer_store_use_getters->savelayer();
}
