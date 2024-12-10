#include "storage/storage/layers.h"
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
std::vector<IDMap> copyIDMap(const std::vector<IDMap>& idmap) {
    std::vector<IDMap> m;
    if (!idmap.empty()) {
        m.reserve(idmap.size());  // 预分配空间，避免多次动态分配
        m.insert(m.end(), idmap.begin(), idmap.end());  // 复制内容
    }
    return m;
}
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
        boost::filesystem::path rundir(rundir);
        if (!boost::filesystem::exists(rundir)) {
            if (!boost::filesystem::create_directories(rundir)) {
                throw myerror("Failed to create rundir: " + rundir.string());
            }
        }

        boost::filesystem::path layerdir(layerdir);
        if (!boost::filesystem::exists(layerdir)) {
            if (!boost::filesystem::create_directories(layerdir)) {
                throw myerror("Failed to create layerdir: "+layerdir.string());
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
                throw myerror("Duplicate Name: " + name);
            }
        }

        // 处理父层信息
        std::string parent = "";
        if (parentLayer != nullptr) {
            parent = parentLayer->ID;
        }
        // 2. 定义相关变量
        // 	var (
        // 	templateIDMappings         *idtools.IDMappings
        // 	templateMetadata           string
        // 	templateCompressedDigest   digest.Digest
        // 	templateCompressedSize     int64
        // 	templateUncompressedDigest digest.Digest
        // 	templateTOCDigest          digest.Digest
        // 	templateUncompressedSize   int64
        // 	templateCompressionType    archive.Compression
        // 	templateUIDs, templateGIDs []uint32
        // 	templateTSdata             []byte
        // )
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
        throw myerror("Filesystem error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw myerror("Error: " + std::string(e.what()));
    }
}
/**
 * @brief 加载layer
 * 
 * @param lockedForWriting 
 * @return true 
 * @return false 
 */
bool layerStore::load(bool lockedForWriting){
    //比较复杂
    return false;
}