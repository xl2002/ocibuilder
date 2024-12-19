#if !defined(STORAGE_STORAGE_LAYERS_H)
#define STORAGE_STORAGE_LAYERS_H
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/thread.hpp>
#include <unordered_set>
#include "storage/lockfile/lockfile.h"
#include "storage/lockfile/lastwrite.h"
#include "image/types/define/types.h"
#include "image/types/define/build.h"
#include "filesys/utils/idtools.h"
#include "storage/storage/stringid.h"
#include "storage/storage/store.h"
#include "utils/common/json.h"
#include <boost/optional.hpp>
#include <boost/json.hpp>
#include "utils/common/go/string.h"
using std::string;
using std::vector;
struct LayerOptions;
class TruncIndex;
struct IDMap;
class Driver;
class Compression;
struct Layer{
    public:
    // ID 是创建时指定的，或由库生成的随机值
    std::string ID;

    // Names 是用户定义的可选便捷值，名称在层中是唯一的
    std::vector<std::string> Names;

    // Parent 是此层继承数据的上一层的 ID
    std::string Parent;

    // Metadata 是供调用者方便使用的数据，不应过大
    std::string Metadata;

    // MountLabel 是挂载层时使用的 SELinux 标签
    std::string MountLabel;

    // MountPoint 是此层挂载的路径或最近一次挂载的路径
    std::string MountPoint;

    // MountCount 是容器层挂载点的引用计数
    int mountCount=0;

    // Created 是此层创建时的时间戳
    std::chrono::system_clock::time_point Created;

    // CompressedDigest 是最后传递给 ApplyDiff() 或 create() 的压缩数据的摘要
    std::shared_ptr<Digest> CompressedDigest=std::make_shared<Digest>();

    // CompressedSize 是最后传递的压缩数据的长度
    int64_t compressedSize=0;

    // UncompressedDigest 是解压缩后的数据摘要
    std::shared_ptr<Digest> UncompressedDigest=std::make_shared<Digest>();

    // TOCDigest 是 TOC 的摘要，在部分拉取镜像时使用
    std::shared_ptr<Digest> TOCDigest=std::make_shared<Digest>();

    // UncompressedSize 是解压缩后的数据长度
    int64_t uncompressedSize=0;

    // CompressionType 是检测到的压缩类型
    std::shared_ptr<Compression> compressionType=std::make_shared<Compression>();

    // UIDs 和 GIDs 是此层中使用的 UID 和 GID 列表
    std::vector<uint32_t> UIDs;
    std::vector<uint32_t> GIDs;

    // Flags 是关于此层的任意数据
    std::map<std::string, boost::optional<std::string>> Flags;

    // UIDMap 和 GIDMap 用于设置用户命名空间中使用的映射
    std::vector<IDMap> UIDMap;
    std::vector<IDMap> GIDMap;

    // ReadOnly 表示此层是否位于只读存储中
    bool readOnly=false;

    // volatileStore 表示容器是否来自临时 JSON 文件
    bool volatileStore=false;

    // BigDataNames 是存储供调用者使用的大数据项名称
    std::vector<std::string> BigDataNames;
    // 添加友元函数 tag_invoke 进行序列化
    friend void tag_invoke(const boost::json::value_from_tag, boost::json::value& jv, const Layer& layer) {
        jv=boost::json::object{
            {"ID", layer.ID},
            {"Parent", layer.Parent},
            {"Names", boost::json::array(layer.Names.begin(), layer.Names.end())},
            {"Created", timePointToISOString(layer.Created)},
            // {"Metadata", layer.Metadata},
            // {"MountLabel", layer.MountLabel},
            // {"MountPoint", layer.MountPoint},
            // {"mountCount", layer.mountCount},
            {"compressedSize", layer.compressedSize},
            {"uncompressedSize", layer.uncompressedSize},
            // {"readOnly", layer.readOnly},
            // {"volatileStore", layer.volatileStore}
            // {"Names", boost::json::array(layer.Names.begin(), layer.Names.end())},
        };
    }
    friend Layer tag_invoke(boost::json::value_to_tag<Layer>, const boost::json::value& jv) {
        const auto& obj = jv.as_object();
        Layer layer;
        layer.ID = obj.at("ID").as_string().c_str();
        layer.Parent = obj.at("Parent").as_string().c_str();
        layer.Names = boost::json::value_to<std::vector<std::string>>(obj.at("Names"));
        layer.Created = parseISOStringToTimePoint(obj.at("Created").as_string().c_str());
    //     layer.Metadata = obj.at("Metadata").as_string().c_str();
    //     layer.MountLabel = obj.at("MountLabel").as_string().c_str();
    //     layer.MountPoint = obj.at("MountPoint").as_string().c_str();
    //     layer.mountCount = obj.at("mountCount").as_int64();
        layer.compressedSize = obj.at("compressedSize").as_int64();
        layer.uncompressedSize = obj.at("uncompressedSize").as_int64();
    //     layer.readOnly = obj.at("readOnly").as_bool();
    //     layer.volatileStore = obj.at("volatileStore").as_bool();
        return layer;
    }
};
class lockFile;
struct multipleLockFile{
    std::vector<std::shared_ptr<lockFile>>lockfiles;
};
struct rwLayerStore_interface;
struct layerStore:public rwLayerStore_interface{
    std::shared_ptr<multipleLockFile> lockfile=std::make_shared<multipleLockFile>();
    std::shared_ptr<lockFile> mountsLockfile=std::make_shared<lockFile>();
    std::string rundir;
    std::map<int,std::string> jsonPath;
    std::string layerdir;
    mutable boost::shared_mutex inProcessLock; // 可重入读写锁
    lastwrite lastWrite;                    // 最后写入时间
    lastwrite mountsLastWrite;              // 仅在 lockfile.IsReadWrite() 时有效

    // 层存储管理
    std::vector<std::shared_ptr<Layer>> layers;         // 层列表
    std::shared_ptr<TruncIndex> idindex;                // 截断索引
    std::map<std::string, std::shared_ptr<Layer>> byid; // 按 ID 存储的层
    std::map<std::string, std::shared_ptr<Layer>> byname; // 按名称存储的层
    std::map<std::string, std::shared_ptr<Layer>> bymount; // 按挂载点存储的层
    std::map<Digest, std::vector<std::string>> bycompressedsum; // 按压缩摘要存储的层
    std::map<Digest, std::vector<std::string>> byuncompressedsum; // 按未压缩摘要存储的层
    std::map<Digest, std::vector<std::string>> bytocsum; // 按 TOC 摘要存储的层
    // std::array<std::chrono::system_clock::time_point, numLayerLocationIndex> layerspathsModified; // 层路径修改时间
    std::map<int,std::chrono::system_clock::time_point>layerspathsModified;
    // 驱动接口
    std::shared_ptr<Driver> driver=std::make_shared<Driver>();

    std::tuple<std::shared_ptr<Layer>,int64_t> create(
        std::string& id,std::shared_ptr<Layer> parentLayer,std::vector<std::string>names,
        std::string mountLabel,std::map<std::string,std::string> options,
        std::shared_ptr<LayerOptions>moreOptions,bool writeable,
        std::ifstream& diff) override;
    bool load(bool lockedForWriting);
    bool savelayer();
};

struct DiffOptions{
    std::shared_ptr<::Compression> Compression=std::make_shared<::Compression>();
    DiffOptions()=default;
};
#endif // STORAGE_LAYERS_H
