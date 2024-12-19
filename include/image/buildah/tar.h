#if !defined(IMAGE_BUILDAH_TAR_H)
#define IMAGE_BUILDAH_TAR_H
#include <mutex>
#include <thread>
#include <condition_variable>
#include <exception>
#include <memory>
#include <utility>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include "image/buildah/digester.h"
#include "utils/common/error.h"
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
// #include "image/buildah/tar_other.h"
// #include "image/buildah/format.h"
namespace fs = boost::filesystem;
// tar 文件头结构
// struct TarHeader {
//     char filename[100];       // 文件名
//     char mode[8];             // 权限
//     char uid[8];              // 用户 ID
//     char gid[8];              // 组 ID
//     char uname[32];           // 用户名
//     char gname[32];           // 组名
//     char filesize[12];        // 文件大小（八进制，11 位，末尾有空字符）
//     char mtime[12];           // 修改时间（八进制，12 位）
//     char checksum[8];         // 校验和（填充空格，计算时会替换为正确的值）
//     char typeflag[1];         // 文件类型标志
//     char linkname[100];       // 链接名（对于符号链接使用）
//     char magic[6];            // 魔术标记（ustar）
//     char version[2];          // 版本（00）
//     char uname_group[32];     // 用户名和组名
//     char padding[255];        // 填充，确保结构体总大小为 512 字节

//     // 创建tar文件并写入文件内容
//     std::ofstream createTar(const std::string& tarFilePath, const fs::path& directory);
// };
void createTar(const std::string& tar_path,const std::string& folder_path);
struct tarFilterer
{
    // 等待线程完成
    boost::thread_group wg; // 
    // std::shared_ptr<tarpp::details::TarHeader> tarHeader=std::make_shared<tarpp::details::TarHeader>();
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
std::tuple<std::shared_ptr<Digest>,int> newTarDigester(const std::string& contentType, const std::string& tarFilePath, const fs::path& directory);
#endif // IMAGE_BUILDAH_TAR_H)
