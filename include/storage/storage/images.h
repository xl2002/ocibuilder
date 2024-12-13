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
namespace storage{
    // 表示一个镜像和相关的元数据的结构体
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

        }
        /**
         * @brief 序列化
         * 
         * @param jv 
         * @param image 
         */
        friend Image tag_invoke(boost::json::value_to_tag<Image>, const boost::json::value& jv){

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
                {"size", m.size}
            };
        }

        // 反序列化
        friend manifest tag_invoke(boost::json::value_to_tag<manifest>, const boost::json::value& jv) {
            const auto& obj = jv.as_object();
            manifest m;
            m.mediaType = obj.at("mediaType").as_string().c_str();  // 使用 as_string() 而不是 c_str()
            m.digest = obj.at("digest").as_string().c_str();  // 使用 as_string() 而不是 c_str()
            m.size = obj.at("size").as_uint64();
            return m;
        }
    };
    struct index{
        std::string schemaVersion;
        std::vector<manifest> manifests;
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
            };
        }
        //反序列化
        friend index tag_invoke(boost::json::value_to_tag<index>, const boost::json::value& jv) {
            const auto& obj = jv.as_object();
            index image;
            image.schemaVersion = obj.at("schemaVersion").as_string().c_str();
            image.manifests = boost::json::value_to<std::vector<manifest>>(obj.at("manifests"));
            return image;
        }
    };
}
#endif // STORAGE_IMAGES_H)
