#include "image/buildah/tar.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
#include <zlib/zlib.h>
#include <boost/algorithm/string.hpp>
#include "utils/logger/ProcessSafeLogger.h"
namespace fs = boost::filesystem;

// 归档文件时的固定时间戳（1970-01-01 00:00:00 UTC）
const time_t FIXED_MTIME = 0;

/**
 * @brief 将单个文件添加到tar归档中
 * 
 * @param a libarchive归档对象指针
 * @param file_path 要添加的文件的完整路径
 * @param base_path 基础路径，用于计算相对路径
 * 
 * @details 该函数负责:
 * 1. 获取文件状态信息
 * 2. 创建相对于base_path的相对路径
 * 3. 设置归档条目属性(大小、权限、时间戳等)
 * 4. 写入文件内容到归档
 * 
 * @note 如果文件状态获取失败或写入出错会输出错误信息
 * @see createTar
 */
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
        logger->log_error("Failed to write header for " + relative_path + ": " + archive_error_string(a));
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
/**
 * @brief 创建tar归档文件
 * 
 * @param tarFilePath 要创建的tar文件路径
 * @param directory 要打包的目录路径
 * 
 * @details 该函数实现完整的tar归档创建流程:
 * 1. 验证目录存在性
 * 2. 初始化libarchive对象
 * 3. 设置tar格式
 * 4. 递归遍历目录结构
 * 5. 调用add_file_to_archive添加每个文件
 * 6. 处理目录条目
 * 7. 关闭归档
 * 
 * @throw fs::filesystem_error 如果目录遍历出错
 * @note 会在控制台输出操作进度信息
 * @see add_file_to_archive
 */

void createTar(const std::string& tarFilePath, const fs::path& directory) {
    logger->log_info("Starting to create tar archive: "+tarFilePath+" from directory: "+directory.string().c_str());
             
    if (!fs::exists(directory)) {
        std::cerr << "Error: Directory " << directory << " does not exist." << std::endl;
        logger->log_error("Directory does not exist: " + directory.string());
        return;
    }

    struct archive* a = archive_write_new();
    if (!a) {
        std::cerr << "Error: Failed to create archive object." << std::endl;
        logger->log_error("Failed to create archive object");
        return;
    }

    // 设定为 ustar 格式（兼容 OCI/Docker）
    // archive_write_set_format_pax_restricted(a);
    archive_write_set_format_ustar(a);
    archive_write_set_bytes_per_block(a, 512); // Docker 规范: 512 字节块
    archive_write_set_bytes_in_last_block(a, 1); // 确保写入最后一个块
    if (archive_write_open_filename(a, tarFilePath.c_str()) != ARCHIVE_OK) {
        std::cerr << "Error opening output file " << tarFilePath << ": " 
                  << archive_error_string(a) << std::endl;
        logger->log_error("Failed to open output file " + tarFilePath + ": " + archive_error_string(a));
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
        logger->log_error("Failed to close archive: " + std::string(archive_error_string(a)));
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
        logger->log_error("Error while creating tar: "+std::string(e.what()));
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
/**
 * @brief 关闭tar过滤器关联的文件流
 * 
 * @details 安全关闭pipeWriter_TarHeader文件流，
 * 如果流已经关闭则不做任何操作
 */
void tarFilterer::Close() {
    // 检查文件流是否已经打开
    if (pipeWriter_TarHeader.is_open()) {
        // 关闭文件流
        pipeWriter_TarHeader.close();
    }
}

/**
 * @brief 获取内容类型
 * 
 * @return std::string 返回嵌套digester的内容类型
 * 
 * @see digester_interface::ContentType
 */
std::string tarDigester::ContentType(){
    return this->nested->ContentType();
}
/**
 * @brief 向hash中写入数据计算sha256
 * 
 * @param data 
 */
/**
 * @brief 写入数据到摘要计算器
 * 
 * @param data 要写入的数据
 * 
 * @details 将数据转发给嵌套的digester进行计算
 */
void tarDigester::write(const std::string& data){
    this->nested->write(data);
}

/**
 * @brief 关闭摘要计算器
 * 
 * @details 关闭关联的tar过滤器并标记为已关闭状态
 */
void tarDigester::close(){
    if(this->isOpen){
        this->isOpen=false;
        this->tarFilterer->Close();
    }
    return;
}
/**
 * @brief 获取计算完成的摘要
 * 
 * @return std::shared_ptr<::Digest> 摘要对象指针
 * 
 * @details 从嵌套digester获取计算结果
 */
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
/**
 * @brief 创建新的tar摘要计算器
 * 
 * @param contentType 内容类型字符串
 * @param tarFilePath 要创建的tar文件路径
 * @param directory 要打包的目录路径
 * @return std::tuple<std::shared_ptr<Digest>,int> 包含摘要对象和文件长度的元组
 * 
 * @details 该函数:
 * 1. 创建基础digester
 * 2. 创建tar过滤器并生成tar文件
 * 3. 计算tar文件摘要
 * 4. 返回摘要和文件大小
 * 
 * @throw myerror 如果tar文件创建失败
 * @see newTarFilterer
 * @see newSimpleDigester
 */
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

bool extractTarGz(const std::string& tarGzPath, const std::string& destDir, 
    const std::string& expectedTarHash = "") {
    // 创建临时文件名用于存储解压后的tar文件
    std::string tempTarPath = tarGzPath + ".temp.tar";
    
    // 第一阶段：使用zlib解压gz部分得到tar文件
    gzFile gzFilePtr = gzopen(tarGzPath.c_str(), "rb");
    if (!gzFilePtr) {
        logger->log_error( "Error: Failed to open gzip file: "+tarGzPath);
        std::cerr << "Error: Failed to open gzip file: " << tarGzPath << std::endl;
        return false;
    }
    
    std::ofstream tempTarFile(tempTarPath, std::ios::binary);
    if (!tempTarFile) {
        logger->log_error("Error: Failed to create temporary tar file: "+tempTarPath);
        std::cerr << "Error: Failed to create temporary tar file: " << tempTarPath << std::endl;
        gzclose(gzFilePtr);
        return false;
    }
    
    // 解压gz文件到临时tar文件
    constexpr int BUFFER_SIZE = 16384;
    char buffer[BUFFER_SIZE];
    int bytesRead;
    
    while ((bytesRead = gzread(gzFilePtr, buffer, BUFFER_SIZE)) > 0) {
        tempTarFile.write(buffer, bytesRead);
    }
    
    gzclose(gzFilePtr);
    tempTarFile.close();
    // 第二阶段：计算解压出的tar文件的SHA256值并校验
    if (!expectedTarHash.empty()) {
        std::ifstream tarFile(tempTarPath, std::ios::binary);
        if (!tarFile) {
            logger->log_error("Error: Failed to open temporary tar file: "+tempTarPath);
            std::cerr << "Error: Failed to open temporary tar file: " << tempTarPath << std::endl;
            return false;
        }
        
        // SHA256_CTX sha256;
        // if (!SHA256_Init(&sha256)) {
        //     std::cerr << "Error: SHA256 initialization failed" << std::endl;
        //     return false;
        // }
        
        // while (tarFile) {
        //     tarFile.read(buffer, BUFFER_SIZE);
        //     size_t bytesRead = tarFile.gcount();
        //     if (bytesRead > 0) {
        //         if (!SHA256_Update(&sha256, buffer, bytesRead)) {
        //             std::cerr << "Error: SHA256 update failed" << std::endl;
        //             return false;
        //         }
        //     }
        // }
        
        // unsigned char hash[SHA256_DIGEST_LENGTH];
        // if (!SHA256_Final(hash, &sha256)) {
        //     std::cerr << "Error: SHA256 finalization failed" << std::endl;
        //     return false;
        // }
        
        // std::stringstream ss;
        // for(unsigned char c : hash) {
        //     ss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        // }
        
        std::string actualHash = Fromfile(tempTarPath)->digest.substr(7);
        
        if (actualHash != expectedTarHash) {
            logger->log_error("Error: SHA256 verification failed!");
            logger->log_error("  Expected: " + expectedTarHash+"  Actual:   " + actualHash);
            std::cerr << "Error: SHA256 verification failed!" << std::endl;
            std::cerr << "  Expected: " << expectedTarHash << std::endl;
            std::cerr << "  Actual:   " << actualHash << std::endl;
            // 清理临时文件
            std::remove(tempTarPath.c_str());
            return false;
        }
        logger->log_info("SHA256 verification passed for tar file.");
        std::cout << "SHA256 verification passed for tar file." << std::endl;
    }
    
    // 第三阶段：使用libarchive解压tar文件
    struct archive *a = nullptr;
    struct archive *ext = nullptr;
    struct archive_entry *entry = nullptr;
    int r;

    // 创建archive reader，这次只需要支持tar格式
    a = archive_read_new();
    if (!a) {
        logger->log_error("Error: Failed to create archive reader object.");
        std::cerr << "Error: Failed to create archive reader object." << std::endl;
        // 清理临时文件
        std::remove(tempTarPath.c_str());
        return false;
    }
    
    archive_read_support_format_tar(a); // 只需要支持tar格式
    // 不需要支持压缩，因为我们已经解压过了
    
    // 创建archive writer
    ext = archive_write_disk_new();
    if (!ext) {
        logger->log_error("Error: Failed to create archive writer object.");
        std::cerr << "Error: Failed to create archive writer object." << std::endl;
        archive_read_free(a);
        // 清理临时文件
        std::remove(tempTarPath.c_str());
        return false;
    }
    
    archive_write_disk_set_options(ext, 
        ARCHIVE_EXTRACT_TIME |
        ARCHIVE_EXTRACT_PERM |
        ARCHIVE_EXTRACT_ACL |
        ARCHIVE_EXTRACT_FFLAGS);
    archive_write_disk_set_standard_lookup(ext);

    // 打开临时tar文件
    r = archive_read_open_filename(a, tempTarPath.c_str(), 10240);
    if (r != ARCHIVE_OK) {
        std::cerr << "Error: Failed to open tar file '" << tempTarPath << "': " 
                  << archive_error_string(a) << std::endl;
        logger->log_error("Failed to open tar file " + tempTarPath + ": " + archive_error_string(a));
        archive_read_free(a);
        archive_write_free(ext);
        // 清理临时文件
        std::remove(tempTarPath.c_str());
        return false;
    }

    // 处理tar文件中的每个条目
    while ((r = archive_read_next_header(a, &entry)) == ARCHIVE_OK) {
        const char* entryPath = archive_entry_pathname(entry);
        if (!entryPath) {
            logger->log_error ("Warning: Skipping entry with null path.");
            std::cerr << "Warning: Skipping entry with null path." << std::endl;
            continue;
        }

        // 构建目标路径
        std::string destPath = destDir + "/" + entryPath;
        
        // 处理Windows路径分隔符
        #ifdef _WIN32
        for (size_t i = 0; i < destPath.length(); ++i) {
            if (destPath[i] == '/')
                destPath[i] = '\\';
        }
        #endif

        // 设置条目的目标路径
        archive_entry_set_pathname(entry, destPath.c_str());
        
        // 写入头部信息（创建目录/文件）
        r = archive_write_header(ext, entry);
        if (r != ARCHIVE_OK) {
            std::cerr << "Error: Failed to write header for entry '" << entryPath 
                      << "': " << archive_error_string(ext) << std::endl;
            logger->log_error("Failed to write header for entry " + std::string(entryPath) + 
                            ": " + archive_error_string(ext));
            continue;
        }
            
        if (archive_entry_size(entry) > 0) {
            const void* buff;
            size_t size;
            int64_t offset;
            
            while (true) {
                r = archive_read_data_block(a, &buff, &size, &offset);
                if (r == ARCHIVE_EOF)
                    break;
                if (r != ARCHIVE_OK) {
                    std::cerr << "Error: Failed to read data for entry '" << entryPath 
                              << "': " << archive_error_string(a) << std::endl;
                    logger->log_error("Failed to read data for entry " + std::string(entryPath) + 
                                    ": " + archive_error_string(a));
                    archive_read_close(a);
                    archive_read_free(a);
                    archive_write_close(ext);
                    archive_write_free(ext);
                    // 清理临时文件
                    std::remove(tempTarPath.c_str());
                    return false;
                }
                    
                r = archive_write_data_block(ext, buff, size, offset);
                if (r != ARCHIVE_OK) {
                    std::cerr << "Error: Failed to write data for entry '" << entryPath 
                              << "': " << archive_error_string(ext) << std::endl;
                    logger->log_error("Failed to write data for entry " + std::string(entryPath) + 
                                    ": " + archive_error_string(ext));
                    archive_read_close(a);
                    archive_read_free(a);
                    archive_write_close(ext);
                    archive_write_free(ext);
                    // 清理临时文件
                    std::remove(tempTarPath.c_str());
                    return false;
                }
            }
        }
        
        r = archive_write_finish_entry(ext);
        if (r != ARCHIVE_OK) {
            std::cerr << "Error: Failed to finish writing entry '" << entryPath 
                      << "': " << archive_error_string(ext) << std::endl;
            logger->log_error("Error: Failed to finish writing entry '" + std::string(entryPath) + "': "+ archive_error_string(ext));
            archive_read_close(a);
            archive_read_free(a);
            archive_write_close(ext);
            archive_write_free(ext);
            // 清理临时文件
            std::remove(tempTarPath.c_str());
            return false;
        }
    }

    if (r != ARCHIVE_EOF) {
        logger->log_error("Error: Unexpected error while reading tar archive: " + std::string(archive_error_string(a)));
        std::cerr << "Error: Unexpected error while reading tar archive: " 
                  << archive_error_string(a) << std::endl;
    }

    // 清理资源
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    
    // 删除临时tar文件
    std::remove(tempTarPath.c_str());
    
    return (r == ARCHIVE_EOF);
}
