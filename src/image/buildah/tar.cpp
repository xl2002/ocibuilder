#include "image/buildah/tar.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
#include <boost/algorithm/string.hpp>
namespace fs = boost::filesystem;

/**
 * @brief 创建新tar文件，将directory下的文件写入,返回文件流
 * 
 * @param tarFilePath 
 * @param directory 
 * @return std::ofstream 
 */

// void add_file_to_archive(struct archive* a, const fs::path& file_path, const fs::path& base_path) {
//     struct archive_entry* entry;
//     struct stat st;

//     // 获取文件信息
//     if (stat(file_path.string().c_str(), &st)) {
//         std::cerr << "Error: Cannot stat file " << file_path << std::endl;
//         return;
//     }

//     // 创建相对路径
//     std::string relative_path = fs::relative(file_path, base_path).string();
//     // std::string relative_path = fs::relative(file_path, base_path).generic_string();
//     std::replace(relative_path.begin(), relative_path.end(), '\\', '/'); 
//     // 创建archive entry
//     entry = archive_entry_new();
//     archive_entry_set_pathname(entry, relative_path.c_str());
//     archive_entry_set_size(entry, st.st_size);
//     archive_entry_set_filetype(entry, AE_IFREG);
//     archive_entry_set_perm(entry, 0644);

//     // 设置时间戳
//     archive_entry_set_uid(entry, 0);    // 强制 UID=0
//     archive_entry_set_gid(entry, 0);    // 强制 GID=0
//     // archive_entry_set_mtime(entry, st.st_mtime, 0);
//     archive_entry_set_mtime(entry, 0, 0);

//     // 写入entry头信息
//     if (archive_write_header(a, entry) != ARCHIVE_OK) {
//         std::cerr << "Error writing header for " << relative_path << ": " 
//                   << archive_error_string(a) << std::endl;
//         archive_entry_free(entry);
//         return;
//     }

//     // 写入文件内容（仅对普通文件）
//     if (S_ISREG(st.st_mode)) {
//         char buff[8192];
//         FILE* file = fopen(file_path.string().c_str(), "rb");
//         if (file) {
//             size_t len = fread(buff, 1, sizeof(buff), file);
//             while (len > 0) {
//                 archive_write_data(a, buff, len);
//                 len = fread(buff, 1, sizeof(buff), file);
//             }
//             fclose(file);
//         }
//     }

//     archive_entry_free(entry);
// }
// void createTar(const std::string& tarFilePath, const fs::path& directory)
// {
//     // 1. 验证目录是否存在
//     if (!fs::exists(directory)) {
//         std::cerr << "Error: Directory " << directory << " does not exist." << std::endl;
//         return;
//     }

//     // 2. 创建archive对象
//     struct archive* a = archive_write_new();
//     if (!a) {
//         std::cerr << "Error: Failed to create archive object." << std::endl;
//         return;
//     }

//     // 3. 设置tar格式
//     if (archive_write_set_format_ustar(a) != ARCHIVE_OK) {
//         std::cerr << "Error setting tar format: " << archive_error_string(a) << std::endl;
//         archive_write_free(a);
//         return;
//     }

//     // 4. 打开输出文件
//     if (archive_write_open_filename(a, tarFilePath.c_str()) != ARCHIVE_OK) {
//         std::cerr << "Error opening output file " << tarFilePath << ": " 
//                   << archive_error_string(a) << std::endl;
//         archive_write_free(a);
//         return;
//     }

//     std::cout << "Creating tar archive: " << tarFilePath << std::endl;
//     std::cout << "Adding files from directory: " << directory << std::endl;

//     // 5. 递归遍历目录并添加文件
//     try {
//         for (const auto& entry : fs::recursive_directory_iterator(directory)) {
//             if (fs::is_regular_file(entry.status())) {
//                 add_file_to_archive(a, entry.path(), directory);
//             } else if (fs::is_directory(entry.status())) {
//                 // 为目录创建条目（确保空目录也被包含）
//                 struct archive_entry* dir_entry = archive_entry_new();
//                 std::string relative_path = fs::relative(entry.path(), directory).string() + "/";
//                 // std::string relative_path = fs::relative(entry.path(), directory).generic_string() + "/";
//                 std::replace(relative_path.begin(), relative_path.end(), '\\', '/'); 
//                 archive_entry_set_pathname(dir_entry, relative_path.c_str());
//                 archive_entry_set_filetype(dir_entry, AE_IFDIR);
//                 archive_entry_set_perm(dir_entry, 0755);
//                 archive_entry_set_uid(dir_entry, 0);
//                 archive_entry_set_gid(dir_entry, 0);
//                 archive_entry_set_mtime(dir_entry, 0, 0);
//                 archive_write_header(a, dir_entry);
//                 archive_entry_free(dir_entry);
//             }
//         }
//     } catch (const fs::filesystem_error& e) {
//         std::cerr << "Filesystem error: " << e.what() << std::endl;
//     }

//     // 6. 关闭archive
//     if (archive_write_close(a) != ARCHIVE_OK) {
//         std::cerr << "Error closing archive: " << archive_error_string(a) << std::endl;
//     }
//     archive_write_free(a);

//     std::cout << "Successfully created tar archive: " << tarFilePath << std::endl;
// }


// 归档文件时的固定时间戳（1970-01-01 00:00:00 UTC）
const time_t FIXED_MTIME = 0;

void add_file_to_archive(struct archive* a, const fs::path& file_path, const fs::path& base_path) {
    struct archive_entry* entry;
    struct stat st;

    // 获取文件信息
    if (stat(file_path.string().c_str(), &st) != 0) {
        std::cerr << "Error: Cannot stat file " << file_path << std::endl;
        return;
    }

    // 计算相对路径
    std::string relative_path = fs::relative(file_path, base_path).string();
    boost::replace_all(relative_path, "\\", "/"); // 统一使用 `/` 分隔符

    // 创建 archive entry
    entry = archive_entry_new();
    archive_entry_set_pathname(entry, relative_path.c_str());
    archive_entry_set_uid(entry, 0);  // 设为 root 用户
    archive_entry_set_gid(entry, 0);  // 设为 root 组
    archive_entry_set_mtime(entry, FIXED_MTIME, 0);
    // archive_entry_set_mtime(entry, st.st_mtime, 0);
    archive_entry_unset_atime(entry); // 取消 `atime`
    archive_entry_unset_ctime(entry); // 取消 `ctime`
    // 处理不同的文件类型
    if (S_ISREG(st.st_mode)) {
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
    } else if (S_ISDIR(st.st_mode)) {
        archive_entry_set_filetype(entry, AE_IFDIR);
        archive_entry_set_perm(entry, 0755);
    } 
    archive_entry_set_fflags(entry, 0, 0);
    // 写入 entry 头信息
    if (archive_write_header(a, entry) != ARCHIVE_OK) {
        std::cerr << "Error writing header for " << relative_path << ": " 
                  << archive_error_string(a) << std::endl;
        archive_entry_free(entry);
        return;
    }

    // 写入文件内容（仅普通文件）
    if (S_ISREG(st.st_mode)) {
        char buff[8192];
        std::ifstream file(file_path.string(), std::ios::binary);
        while (file.read(buff, sizeof(buff)) || file.gcount() > 0) {
            archive_write_data(a, buff, file.gcount());
        }
    }

    archive_entry_free(entry);
}

void createTar(const std::string& tarFilePath, const fs::path& directory) {
    if (!fs::exists(directory)) {
        std::cerr << "Error: Directory " << directory << " does not exist." << std::endl;
        return;
    }

    struct archive* a = archive_write_new();
    if (!a) {
        std::cerr << "Error: Failed to create archive object." << std::endl;
        return;
    }

    // 设定为 PAX 格式（兼容 OCI/Docker）
    // archive_write_set_format_pax_restricted(a);
    archive_write_set_format_ustar(a);
    archive_write_set_bytes_per_block(a, 512); // Docker 规范: 512 字节块
    archive_write_set_bytes_in_last_block(a, 1); // 确保写入最后一个块
    if (archive_write_open_filename(a, tarFilePath.c_str()) != ARCHIVE_OK) {
        std::cerr << "Error opening output file " << tarFilePath << ": " 
                  << archive_error_string(a) << std::endl;
        archive_write_free(a);
        return;
    }

    std::vector<std::string> paths;
    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        paths.push_back(entry.path().string());
    }

    // **使用稳定的字符串排序**
    std::sort(paths.begin(), paths.end());

    for (const auto& path_str : paths) {
        add_file_to_archive(a, fs::path(path_str), directory);
    }

    if (archive_write_close(a) != ARCHIVE_OK) {
        std::cerr << "Error closing archive: " << archive_error_string(a) << std::endl;
    }
    archive_write_free(a);
}
/**
 * @brief 根据tarfilePath和directory创建tarFilterer
 *  
 * @param tarFilePath 
 * @param directory 
 * @return std::shared_ptr<tarFilterer> 
 */
// 创建 tarFilterer 函数 
std::shared_ptr<tarFilterer> newTarFilterer(const std::string& tarFilePath, const fs::path& directory)
{
    // 创建 tarFilterer 对象
    auto filterer = std::make_shared<tarFilterer>();

    // 直接调用 createTar 函数来创建 tar 文件
    try {
        createTar(tarFilePath, directory.string());
    } catch (const myerror& e) {
        std::cerr << "Error while creating tar: " << e.what() << std::endl;
        throw;
    }
    // filterer->pipeWriter_TarHeader.close();
    return filterer;  // 返回创建的 tarFilterer 对象
}


/**
 * @brief 关闭pipeWriter_TarHeader文件流
 * 
 */
void tarFilterer::Close() {
    // 检查文件流是否已经打开
    if (pipeWriter_TarHeader.is_open()) {
        // 关闭文件流
        pipeWriter_TarHeader.close();
    }
}

std::string tarDigester::ContentType(){
    return this->nested->ContentType();
}
/**
 * @brief 向hash中写入数据计算sha256
 * 
 * @param data 
 */
void tarDigester::write(const std::string& data){
    this->nested->write(data);
}

void tarDigester::close(){
    if(this->isOpen){
        this->isOpen=false;
        this->tarFilterer->Close();
    }
    return;
}
std::shared_ptr<::Digest> tarDigester::Digest(){
    return this->nested->Digest();
}

/**
 * @brief 新建tarDigester实例
 * 
 * @param contentType 
 * @return std::shared_ptr<digester_interface> 
 */
// newTarDigester 函数实现
std::tuple<std::shared_ptr<Digest>,int> newTarDigester(const std::string& contentType, const std::string& tarFilePath, const fs::path& directory) {
    // 创建嵌套的 digester
    auto nested = newSimpleDigester(contentType);

    // 创建 tarFilterer 对象并配置它，传入 tarFilePath 和 directory
    auto tarFilterer = newTarFilterer(tarFilePath, directory);

    // 创建并初始化 tarDigester 对象
    auto digester = std::make_shared<tarDigester>();
    digester->isOpen = true;
    digester->nested = nested;

    digester->tarFilterer = tarFilterer;
    auto tardigest=Fromfile(tarFilePath);
    std::ifstream tarFile(tarFilePath);
    std::ostringstream buffer;
    buffer << tarFile.rdbuf();
    // digester->write(buffer.str());
    // auto digester2=FromString(buffer.str());
    // tarFile.close();
    // auto a=digester->Digest()->Encoded();
    return std::make_tuple(tardigest,buffer.str().length());
}