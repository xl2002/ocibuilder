#if !defined(IMAGE_BUILDAH_TAR_H)
#define IMAGE_BUILDAH_TAR_H
#include "image/buildah/digester.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <exception>
#include <memory>
#include <utility>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
// tar 文件头结构
struct TarHeader {
    char filename[100];   // 文件名
    char mode[8];         // 文件权限
    char uid[8];          // 用户 ID
    char gid[8];          // 组 ID
    char filesize[12];    // 文件大小
    char mtime[12];       // 文件修改时间
    char checksum[8];     // 校验和
    char typeflag[1];     // 文件类型标志
    char linkname[100];   // 硬链接名称
    char magic[6];        // tar 文件魔术标记
    char version[2];      // tar 版本号
    char uname[32];       // 用户名
    char gname[32];       // 组名
    char padding[255];    // 填充
    std::ofstream createTar(const std::string& tarFilePath, const fs::path& directory);
};
struct tarFilterer
{
    // 等待线程完成
    boost::thread_group wg; // 
    std::shared_ptr<TarHeader> tarHeader=std::make_shared<TarHeader>();
    std::ofstream pipeWriter_TarHeader; // 
    mutable std::mutex closedLock; // 用于保护 closed 标志
    bool closed=false; // 表示是否已关闭
    // const std::exception* err; // 存储发生的错误
        // 关闭过滤器，释放资源
    // void close() {
    //     std::lock_guard<std::mutex> lock(closedLock);
    //     if (closed) {
    //         throw std::runtime_error("tarFilterer 已关闭");
    //     }
    //     closed = true;
    //     if (pipeWriter) {
    //         pipeWriter->close();
    //     }
    //     wg.join(); // 等待所有线程完成
    // }
    void Close();
};
std::shared_ptr<tarFilterer> newTarFilterer(const std::string& tarFilePath,const fs::path& directory);

struct tarDigester:public digester_interface{
    bool isOpen=false;
    std::shared_ptr<digester_interface> nested=nullptr;
    std::shared_ptr<::tarFilterer> tarFilterer=std::make_shared<::tarFilterer>();
    
    std::string ContentType() override;
    void write(const std::string& data) override;
    void close() override;
    std::shared_ptr<::Digest> Digest() override;
};
std::shared_ptr<digester_interface> newTarDigester(string contentType);
#endif // IMAGE_BUILDAH_TAR_H)
