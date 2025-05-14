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
// #include <libarchive/archive.h>
// #include <libarchive/archive_entry.h>
// #include "image/buildah/tar_other.h"
#include "image/buildah/format.h"
namespace fs = boost::filesystem;
void createTar(const std::string& tarFilePath, const fs::path& directory);
void add_file_to_archive(struct archive* a, const fs::path& file_path, const fs::path& base_path);
bool extractTarGz(const std::string& tarGzPath, const std::string& destDir,const std::string& expectedTarHash);
class tarFilterer
{
    public:
    // 等待线程完成
    boost::thread_group wg; // 
    // std::shared_ptr<tarpp::details::TarHeader> tarHeader=std::make_shared<tarpp::details::TarHeader>();
    std::ofstream pipeWriter_TarHeader; // 
    mutable std::mutex closedLock; // 用于保护 closed 标志
    bool closed=false; // 表示是否已关闭

    void Close();
};

std::shared_ptr<tarFilterer> newTarFilterer(const std::string& tarFilePath,const fs::path& directory);

class tarDigester:public digester_interface{
    public:
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
