#include "storage/storage/layers.h"
#include "utils/common/json.h"
#include "utils/logger/ProcessSafeLogger.h"
#include <boost/filesystem.hpp>

using namespace boost::json;
/**
 * @brief 对字符串向量进行去重处理
 * @param names 包含可能重复字符串的输入向量
 * @return std::vector<std::string> 去重后的字符串向量
 */
std::vector<std::string> dedupeStrings(const std::vector<std::string>& names) {
    std::unordered_set<std::string> seen;  // 用于记录已出现的元素
    std::vector<std::string> deduped;  // 用于保存去重后的结果

    for (const auto& name : names) {
        if (seen.find(name) == seen.end()) {  // 如果名字还未出现
            seen.insert(name);  // 插入到 set 中
            deduped.push_back(name);  // 添加到结果数组中
        }
    }

    return deduped;  // 返回去重后的列表
}
// 转换函数
/**
 * @brief 将LinuxIDMapping向量转换为IDMap向量
 * @param linuxIDMappings 包含Linux ID映射的输入向量
 * @return std::vector<IDMap> 转换后的IDMap向量
 */
std::vector<IDMap> ConvertToIDMap(const std::vector<LinuxIDMapping>& linuxIDMappings) {
    std::vector<IDMap> idMaps;
    idMaps.reserve(linuxIDMappings.size());  // 为减少扩容，提前分配空间

    // 转换过程
    for (const auto& linuxID : linuxIDMappings) {
        IDMap idMap;
        idMap.ContainerID = static_cast<int>(linuxID.ContainerID);
        idMap.HostID = static_cast<int>(linuxID.HostID);
        idMap.Size = static_cast<int>(linuxID.Size);
        idMaps.push_back(idMap);
    }

    return idMaps;
}
/**
 * @brief 深度拷贝IDMap向量
 * @param idmap 要拷贝的IDMap向量
 * @return std::vector<IDMap> 拷贝后的新IDMap向量
 */
std::vector<IDMap> copyIDMap(const std::vector<IDMap>& idmap) {
    std::vector<IDMap> m;
    if (!idmap.empty()) {
        m.reserve(idmap.size());  // 预分配空间，避免多次动态分配
        m.insert(m.end(), idmap.begin(), idmap.end());  // 复制内容
    }
    return m;
}
/**
 * @brief 拷贝字符串到可选字符串的映射
 * @param m 要拷贝的输入映射
 * @return std::map<std::string, boost::optional<std::string>> 拷贝后的新映射
 */
std::map<std::string, boost::optional<std::string>> copyStringInterfaceMap(const std::map<std::string, boost::optional<std::string>>& m) {
    std::map<std::string, boost::optional<std::string>> ret;
    for (const auto& pair : m) {
        ret[pair.first] = pair.second;  // 浅拷贝每个元素
    }
    return ret;
}
/**
 * @brief 新建一个layer在overlay文件中
 * 
 * @param id 
 * @param parentLayer 
 * @param mountLabel 
 * @param options 
 * @param writeable 
 * @param diff 
 * @return std::tuple<std::shared_ptr<Layer>,int64_t> 
 */
std::tuple<std::shared_ptr<Layer>,int64_t> layerStore::create(
        std::string& id,std::shared_ptr<Layer> parentLayer,std::vector<std::string>names,
        std::string mountLabel,std::map<std::string,std::string> options,
        std::shared_ptr<LayerOptions>moreOptions,bool writeable,
        std::ifstream& diff)
{
	try {
        // 使用Boost的filesystem库创建目录
        // boost::filesystem::path rundir(rundir);
        if (!boost::filesystem::exists(rundir)) {
            if (!boost::filesystem::create_directories(rundir)) {
                logger->log_error("Failed to create rundir: " + rundir);
                throw myerror("Failed to create rundir: " + rundir);
            }
        }

        // boost::filesystem::path layerdir(layerdir);
        if (!boost::filesystem::exists(layerdir)) {
            if (!boost::filesystem::create_directories(layerdir)) {
                logger->log_error("Failed to create layerdir: "+layerdir);
                throw myerror("Failed to create layerdir: "+layerdir);
            }
        }
        
	    // 1. GenerateRandomID生成随机ID，判断id没有重复
        // 如果id为空，生成一个随机ID
        if (id.empty()) {
            id = GenerateRandomID();
            while (byid.find(id) != byid.end()) {
                id = GenerateRandomID();
            }
        }
		// 去重 names
        names = dedupeStrings(names);
        
        // 检查是否有重复的 name
        for (const auto& name : names) {
            if (byname.find(name) != byname.end()) {
                logger->log_error("Duplicate Name: " + name);
                throw myerror("Duplicate Name: " + name);
            }
        }

        // 处理父层信息
        std::string parent = "";
        if (parentLayer != nullptr) {
            parent = parentLayer->ID;
        }
        // 2. 定义相关变量

        // 初始化模板层相关变量
        std::shared_ptr<IDMappings> templateIDMappings = nullptr;
        std::string templateMetadata = "";
        std::shared_ptr<Digest> templateCompressedDigest;
        int64_t templateCompressedSize = 0;
        std::shared_ptr<Digest> templateUncompressedDigest;
        std::shared_ptr<Digest> templateTOCDigest;
        int64_t templateUncompressedSize = 0;
        std::shared_ptr<Compression> templateCompressionType;
        std::vector<uint32_t> templateUIDs;
        std::vector<uint32_t> templateGIDs;
        std::vector<uint8_t> templateTSdata;
        templateIDMappings = std::make_shared<IDMappings>();

        // 3. 创建layer
        std::shared_ptr<Layer> layer = std::make_shared<Layer>();

        layer->ID = id;
        layer->Parent = parent;
        layer->Names = names;
        layer->MountLabel = mountLabel;
        layer->Metadata = templateMetadata;
        layer->Created = std::chrono::system_clock::now();  // 使用当前时间作为创建时间
        layer->CompressedDigest = templateCompressedDigest;
        layer->compressedSize = templateCompressedSize;
        layer->UncompressedDigest = templateUncompressedDigest;
        layer->TOCDigest = templateTOCDigest;
        layer->uncompressedSize = templateUncompressedSize;
        layer->compressionType = templateCompressionType;
        layer->UIDs = templateUIDs;
        layer->GIDs = templateGIDs;

        // 将更多选项复制到 Layer 对象
        layer->Flags = copyStringInterfaceMap(moreOptions->flags);
        layer->UIDMap = copyIDMap(ConvertToIDMap(moreOptions->idMappingOptions->UIDMap));
        layer->GIDMap = copyIDMap(ConvertToIDMap(moreOptions->idMappingOptions->GIDMap));
        layer->BigDataNames = {};  // 初始化为空列表
        layer->volatileStore = moreOptions->volatileInfo;

        // 4. 将 Layer 标记为 "incomplete"，表示该层处于未完成状态，后续可能会有清理
        layer->Flags["incomplete"] = "true";  // 在 Layer 的标志中设置 incomplete 标志

        // 将 Layer 添加到 layers 列表中
        int i = 0;
        for(;i<layers.size();i++)
        {
            if(layers[i]->ID==layer->ID)break;
        }
        if(i==layers.size())
        layers.push_back(layer);

        // 将该 Layer 索引到 byid 和 byname 中
        byid[id] = layer;
        for (const auto& name : names) {
            byname[name] = layer;
        }
        // 处理父层信息
        parent = "";
        std::shared_ptr<IDMappings> parentMappings = nullptr;
        std::shared_ptr<IDMappings> oldMappings = nullptr;
        if (parentLayer != nullptr) {
            parent = parentLayer->ID;
            // 使用父层的 UID 和 GID 映射
            std::shared_ptr<IDMappings> parentMappings = NewIDMappingsFromMaps(parentLayer->UIDMap, parentLayer->GIDMap);
        } else {
            // 如果没有父层，初始化空映射
            parentMappings = std::make_shared<IDMappings>();
        }

        // 2. 创建新的 ID 映射
        // 直接将 NewIDMappingsFromMaps 返回的 shared_ptr<IDMappings> 赋值给 idMappings
        std::shared_ptr<IDMappings> idMappings = NewIDMappingsFromMaps(
            ConvertToIDMap(moreOptions->idMappingOptions->UIDMap), 
            ConvertToIDMap(moreOptions->idMappingOptions->GIDMap)
        );
                // 创建CreateOpts对象
        auto opts = std::make_shared<CreateOpts>();
        opts->MountLabel = mountLabel;
        opts->StorageOpt = options;
        opts->IdMappings = idMappings;
        
        // 3. 如果模板层存在，从模板层创建
        if (!moreOptions->templateLayer.empty()) {
        //     if (err = r.driver.CreateFromTemplate(id, moreOptions->TemplateLayer, templateIDMappings, parentLayer->ID, parentMappings, &opts, writeable); err != nullptr) {
        //         cleanupFailureContext = fmt::format("creating a layer from template layer {}", moreOptions->TemplateLayer);
        //         return nullptr, -1, fmt::format("Error creating copy of template layer {} with ID {}: {}", moreOptions->TemplateLayer, id, err);
        //     }
        //     oldMappings = templateIDMappings;
        } 
        else {
            // 4. 如果没有模板层，根据 writeable 标志创建可写或只读层
            if (writeable) {
                driver->CreateReadWrite(id,parent,opts);
            }
            if(parentLayer!=nullptr){
                oldMappings =parentMappings;
            }
        }
        return std::make_tuple(layer,-1);
    } catch (const boost::filesystem::filesystem_error& e) {
        logger->log_error("Filesystem error: " + std::string(e.what()));
        throw myerror("Filesystem error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        logger->log_error("Error: " + std::string(e.what()));
        throw myerror("Error: " + std::string(e.what()));
    }
}
/**
 * @brief 从Layer对象中移除指定名称
 * @param layer 要操作的Layer共享指针
 * @param name 要移除的名称
 * @param tempNames 临时名称映射表
 * @throws myerror 当layer为nullptr时抛出
 */
void removeName(const std::shared_ptr<Layer>& layer, const std::string& name, 
                std::map<std::string, std::shared_ptr<Layer>>& tempNames) {
    if (!layer) {
        logger->log_error("Invalid layer: nullptr provided");
        throw myerror("Invalid layer: nullptr provided");
    }

    // 从 tempNames 中删除映射
    auto it = tempNames.find(name);
    if (it != tempNames.end() && it->second == layer) {
        tempNames.erase(it);
        logger->log_info("Removed name: " + name + " from tempNames");
        std::cout << "Removed name: " << name << " from tempNames" << std::endl;
    } else {
        logger->log_info("Name: " + name + " not found in tempNames for this layer");
        std::cout << "Name: " << name << " not found in tempNames for this layer" << std::endl;
    }

    // 从 layer->Names 中删除
    auto& names = layer->Names;
    auto nameIt = std::find(names.begin(), names.end(), name);
    if (nameIt != names.end()) {
        names.erase(nameIt);
        logger->log_info("Removed name: " + name + " from layer ID: " + layer->ID);
        std::cout << "Removed name: " << name << " from layer ID: " << layer->ID << std::endl;
    } else {
        logger->log_info("Name: " + name + " not found in layer ID: " + layer->ID);
        std::cout << "Name: " << name << " not found in layer ID: " << layer->ID << std::endl;
    }
}
// 从 JSON 字符串解析成 std::vector<std::shared_ptr<Layer>>
/**
 * @brief 从JSON字符串解析Layer对象向量
 * @param jsonData 包含Layer数据的JSON字符串
 * @return std::vector<std::shared_ptr<Layer>> 解析后的Layer对象向量
 * @throws std::exception 当JSON解析失败时抛出
 */
std::vector<std::shared_ptr<Layer>> parseLayersFromJson(const std::string& jsonData) {
    std::vector<std::shared_ptr<Layer>> Layers;

    try {
        // 解析 JSON 数据字符串
        auto layers=unmarshal<std::vector<Layer>>(jsonData);
        for(auto& layer:layers){
            Layers.push_back(std::make_shared<Layer>(layer));
        }

    } catch (const std::exception& ex) {
        logger->log_error("Failed to parse JSON: " + std::string(ex.what()));
        std::cerr << "Failed to parse JSON: " << ex.what() << std::endl;
    }

    return Layers;
}
/**
 * @brief 加载layer
 *  该函数将读取layer store中的所有json文件
 *  并将其解析到Layer对象
 * @param lockedForWriting 
 * @return true 
 * @return false 
 */
// 修改后的 layerStore::load 实现
bool layerStore::load(bool lockedForWriting) {
    try {
        std::vector<std::shared_ptr<Layer>> tempLayers;
        std::map<std::string, std::shared_ptr<Layer>> tempIds;

        for (const auto& [index, path] : jsonPath) {
            // 检查文件是否存在
            boost::filesystem::path jsonFile(path);
            if (!boost::filesystem::exists(jsonFile)) {
                continue; // 文件不存在，跳过
            }

            // 读取文件内容
            std::ifstream file(path);
            if (!file.is_open()) {
                throw myerror("Failed to open file: " + path);
            }
            // 移动文件指针到末尾，计算文件大小
            file.seekg(0, std::ios::end);
            std::streampos fileSize = file.tellg();
            if(fileSize == 0) {
                logger->log_info("the layer store is empty, and we will skip this layer");
                std::cout<< "the layer store is empty, and we will skip this layer" << std::endl;
                return true;
            }
            // 将文件指针重置到开始处
            file.seekg(0, std::ios::beg);
            std::string jsonData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            
            // 调用 unmarshal 函数解析 JSON 数据
            std::vector<std::shared_ptr<Layer>> parsedLayers = parseLayersFromJson(jsonData);

            // 添加层到临时列表
            for (const auto& layer : parsedLayers) {
                if (tempIds.find(layer->ID) != tempIds.end()) {
                    continue; // 忽略重复ID的层
                }
                tempLayers.push_back(layer);
                tempIds[layer->ID] = layer;
            }
        }

        std::map<std::string, std::shared_ptr<Layer>> tempNames;
        std::map<Digest, std::vector<std::string>> tempCompressedSums;
        std::map<Digest, std::vector<std::string>> tempUncompressedSums;

        for (const auto& layer : tempLayers) {
            for (const auto& name : layer->Names) {
                if (tempNames.find(name) != tempNames.end()) {
                    // 从 tempNames 和 layer->Names 中移除名称
                    // removeName(tempNames[name], name, tempNames);
                }
                tempNames[name] = layer;
            }

            if (!layer->CompressedDigest) {
                tempCompressedSums[*layer->CompressedDigest].push_back(layer->ID);
            }
            if (!layer->UncompressedDigest) {
                tempUncompressedSums[*layer->UncompressedDigest].push_back(layer->ID);
            }

        }

        // 将临时数据赋值到正式成员
        layers = std::move(tempLayers);
        byid = std::move(tempIds);
        byname = std::move(tempNames);
        bycompressedsum = std::move(tempCompressedSums);
        byuncompressedsum = std::move(tempUncompressedSums);

        return true;
    } catch (const myerror& e) {
        // 捕获并处理 myerror 类型错误
        logger->log_error("Error in layerStore::load: " + std::string(e.what()));
        std::cerr << "Error in layerStore::load: " << e.what() << std::endl;
        return false;
    }
}
/**
 * @brief 保存Layer对象
 * 
 * 该函数将layers中的Layer对象序列化到JSON字符串
 * 并将其写入到对应的文件中
 * 
 * @return 保存成功返回true，否则返回false
 */
bool layerStore::savelayer() {
    std::vector<Layer> Layers;
    for (const auto& layer : layers) {
        Layers.push_back(*layer);
    }

    // 将Layer对象序列化到JSON字符串
    std::string jsonData = marshal<std::vector<Layer>>(Layers);

    // 遍历jsonPath，写入JSON数据到对应的文件中
    for (const auto& [index, path] : jsonPath) {
        if (!boost::filesystem::exists(path)) {
            continue;
        }
        std::ofstream file(path, std::ios::trunc);
        if (!file.is_open()) {
            return false;
        }
        file << jsonData;
        file.close();
    }

    return true;
}
/**
 * @brief 删除Layer对象
 * 
 * 根据给定的Layer ID，删除存储中的对应Layer对象
 * 
 * @param layerID 要删除的Layer ID
 */
void layerStore::deleteLayer(std::string layerID) {
    auto layer = this->lookup(layerID);
    if (layer == nullptr) {
        return; // 如果不存在该Layer对象，则返回
    }

    // 删除layers中的记录
    for (auto i = 0; i < layers.size(); i++) {
        if (layers[i]->ID == layerID) {
            layers.erase(layers.begin() + i);
            break;
        }
    }

    auto id = layer->ID;

    // 删除byid中的记录
    byid.erase(id);

    // 删除byname中的记录
    for (auto i = 0; i < layer->Names.size(); i++) {
        byname.erase(layer->Names[i]);
    }

    // 删除bycompressedsum中的记录
    if (layer->CompressedDigest != nullptr) {
        bycompressedsum.erase(*layer->CompressedDigest);
    }

    // 删除byuncompressedsum中的记录
    if (layer->UncompressedDigest != nullptr) {
        byuncompressedsum.erase(*layer->UncompressedDigest);
    }
}

/**
 * @brief 查找Layer对象
 * 
 * 根据给定的ID在存储中查找对应的Layer对象。
 * 
 * @param id 要查找的Layer ID
 * @return std::shared_ptr<Layer> 如果找到则返回Layer对象，否则返回nullptr
 */
std::shared_ptr<Layer> layerStore::lookup(const std::string& id) {
    // 在 byid 映射中查找 ID
    auto it = byid.find(id);
    if (it != byid.end()) {
        // 如果找到，则返回对应的 Layer 对象
        return it->second;
    }
    
    // 如果未在 byid 中找到，则在 byname 映射中查找 ID
    auto name = byname.find(id);
    if (name != byname.end()) {
        // 如果找到，则返回对应的 Layer 对象
        return name->second;
    }
    
    // 如果在 byid 和 byname 中都未找到，返回空指针
    return nullptr;
}
