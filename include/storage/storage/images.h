#if !defined(STORAGE_IMAGES_H)
#define STORAGE_IMAGES_H
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include "cobra/error.h"
#include <set>
#include "image/digest/digest.h"
#include "lockfile/lockfile.h"
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
    };
    // struct ImageStore:public roImageStore_interface{
    //     // std::shared_ptr<::lockFile> lockfile=std::make_shared<::lockFile>();
    //     std::string dir;
    //         // 以下字段需使用 inProcessLock 进行同步访问
    //     // mutable std::shared_timed_mutex inProcessLock; // C++11 的读写锁

    //     // 需持有 inProcessLock 的读/写锁才能读/写以下字段
    //     // LastWrite lastWrite;
    //     std::vector<std::shared_ptr<Image>> images;
    //     // TruncIndex* idindex;
    //     std::map<std::string, std::shared_ptr<Image>> byid;
    //     std::map<std::string, std::shared_ptr<Image>> byname;
    //     std::map<Digest, std::vector<std::shared_ptr<Image>>> bydigest;
    //     std::shared_ptr<Image> Get(const std::string& id) override;
    //     std::shared_ptr<Image> lookup(const std::string& id);
    // };
}
#endif // STORAGE_IMAGES_H)
