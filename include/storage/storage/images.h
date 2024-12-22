#if !defined(STORAGE_STORAGE_IMAGES_H)
#define STORAGE_STORAGE_IMAGES_H
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include "utils/common/error.h"
#include <set>
#include "image/digest/digest.h"
#include "storage/lockfile/lockfile.h"
#include <boost/json.hpp>
#include <boost/array.hpp>
#include "utils/common/go/string.h"
#include "image/image_types/v1/config.h"
struct Manifest;
namespace v1{
    struct Image;
}
namespace storage{
    struct manifest;
}
namespace storage{
    // 表示一个镜像和相关的元数据的结构体
    struct index;
    struct Image {
        // ID 是在创建时指定的，或者由库随机生成的值
        std::string ID;

        // Digest 是在创建时指定的摘要值，可用于定位镜像
        std::shared_ptr<Digest> digest=std::make_shared<Digest>(); // 需要定义或包含 digest 库

        // Digests 是镜像清单的摘要值列表，可用于定位镜像
        // 如果Digest被设置，其值也会包含在此列表中
        std::vector<Digest> Digests;

        // Names 是用户定义的名称集合，镜像可以通过其 ID 或任何名称来引用
        std::vector<std::string> Names;

        // NamesHistory 是镜像过去拥有的名称集合，最新的条目位于第一位
        std::vector<std::string> NamesHistory;

        // TopLayer 是镜像最顶层的ID，如果镜像包含一层或多层
        std::string TopLayer;
        std::shared_ptr<storage::manifest> image_index=nullptr;
        // 镜像的manifest
        std::shared_ptr<Manifest> image_manifest=nullptr;
        std::shared_ptr<v1::Image> image_config=nullptr;
        // MappedTopLayers 是顶层的备用版本，它们内容相同但ID映射不同
        std::vector<std::string> MappedTopLayers;

        // Metadata 是为了方便调用者保存的数据，不应太大，因为它常驻内存
        std::string Metadata;

        // BigDataNames 是保存供调用者使用的数据信息的名称列表，这些数据可能很大，只有在从磁盘读取或写入时才加载到内存中
        std::vector<std::string> BigDataNames;

        // BigDataSizes 将 BigDataNames 中的名称映射到已知的存储数据大小
        std::map<std::string, int64_t> BigDataSizes;

        // BigDataDigests 将 BigDataNames 中的名称映射到已知存储数据的摘要值
        std::map<std::string, Digest> BigDataDigests;

        // Created 是镜像创建的时间戳，旧版本库没有追踪此信息
        std::chrono::system_clock::time_point Created;

        // ReadOnly 如果镜像处于只读层存储中，则为 true
        bool ReadOnly=false;

        // Flags 是一个可以存储任意键值对的标志集合
        std::map<std::string, std::string> Flags;
        void recomputeDigests();
        /**
         * @brief 反序列化
         * 
         * @param jv 
         * @param image 
         */
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const Image& image){
            jv = boost::json::object{
                {"ID", image.ID},
                {"digest", image.digest->digest},
                {"Digests", boost::json::value_from(image.Digests)},
                {"Names", boost::json::value_from(image.Names)},
                // {"NamesHistory", boost::json::value_from(image.NamesHistory)},
                {"TopLayer", image.TopLayer},
                // {"MappedTopLayers", boost::json::value_from(image.MappedTopLayers)},
                // {"Metadata", image.Metadata},
                // {"BigDataNames", boost::json::value_from(image.BigDataNames)},
                // {"BigDataSizes", boost::json::value_from(image.BigDataSizes)},
                // {"BigDataDigests", boost::json::value_from(image.BigDataDigests)},
                {"Created", timePointToISOString(image.Created)},
                // {"ReadOnly", image.ReadOnly},
                // {"Flags", boost::json::value_from(image.Flags)}
            };
        }
        /**
         * @brief 序列化
         * 
         * @param jv 
         * @param image 
         */
        friend Image tag_invoke(boost::json::value_to_tag<Image>, const boost::json::value& jv){
            const auto& obj = jv.as_object();
            Image image;
            image.ID = obj.at("ID").as_string().c_str();
            image.digest=std::make_shared<Digest>(obj.at("digest").as_string().c_str());
            image.Digests = boost::json::value_to<std::vector<Digest>>(obj.at("Digests"));
            image.Names = boost::json::value_to<std::vector<std::string>>(obj.at("Names"));
            // image.NamesHistory = boost::json::value_to<std::vector<std::string>>(obj.at("NamesHistory"));
            image.TopLayer = obj.at("TopLayer").as_string().c_str();
            // image.MappedTopLayers = boost::json::value_to<std::vector<std::string>>(obj.at("MappedTopLayers"));
            // image.Metadata = obj.at("Metadata").as_string().c_str();
            // image.BigDataNames = boost::json::value_to<std::vector<std::string>>(obj.at("BigDataNames"));
            // image.BigDataSizes = boost::json::value_to<std::map<std::string, int64_t>>(obj.at("BigDataSizes"));
            // image.BigDataDigests = boost::json::value_to<std::map<std::string, Digest>>(obj.at("BigDataDigests"));
            image.Created = parseISOStringToTimePoint(obj.at("Created").as_string().c_str());
            // image.ReadOnly = obj.at("ReadOnly").as_bool();
            // image.Flags = boost::json::value_to<std::map<std::string, std::string>>(obj.at("Flags"));
            return image;
        }
    };
    struct manifest{
        std::string mediaType;
        std::string digest;
        uint64_t size=0;
        std::map<std::string, std::string> annotations;
        manifest()=default;
        // 序列化
        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const manifest& m) {
            jv = boost::json::object{
                {"mediaType", m.mediaType},
                {"digest", m.digest},
                {"size", m.size},
                {"annotations", boost::json::value_from(m.annotations)}
            };
        }

        // 反序列化
        friend manifest tag_invoke(boost::json::value_to_tag<manifest>, const boost::json::value& jv) {
            const auto& obj = jv.as_object();
            manifest m;
            m.mediaType = obj.at("mediaType").as_string().c_str();  // 使用 as_string() 而不是 c_str()
            m.digest = obj.at("digest").as_string().c_str();  // 使用 as_string() 而不是 c_str()
            // auto s=obj.at("size").get
            m.size = static_cast<uint64_t>(obj.at("size").as_int64());
            m.annotations = boost::json::value_to<std::map<std::string, std::string>>(obj.at("annotations"));
            return m;
        }
    };
    struct index{
        int schemaVersion;
        std::vector<manifest> manifests;
        // std::map<std::string, std::string> annotations;
        index()=default;
        /**
         * @brief 序列化
         * 
         * @param jv 
         * @param image 
         */

        friend void tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const index& image) {
            jv = boost::json::object{
                {"schemaVersion", image.schemaVersion},
                {"manifests", boost::json::value_from(image.manifests)}
                // {"annotations", boost::json::value_from(image.annotations)}
            };
        }
        //反序列化
        friend index tag_invoke(boost::json::value_to_tag<index>, const boost::json::value& jv) {
            const auto& obj = jv.as_object();
            index image;
            image.schemaVersion = obj.at("schemaVersion").as_int64();
            image.manifests = boost::json::value_to<std::vector<manifest>>(obj.at("manifests"));
            // image.annotations = boost::json::value_to<std::map<std::string, std::string>>(obj.at("annotations"));
            return image;
        }
    };
}
#endif // STORAGE_IMAGES_H)
