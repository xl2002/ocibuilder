#include "image/buildah/tar.h"


namespace fs = boost::filesystem;

/**
 * @brief 创建新tar文件，将directory下的文件写入,返回文件流
 * 
 * @param tarFilePath 
 * @param directory 
 * @return std::ofstream 
 */
void createTar(const std::string& tarFilePath, const fs::path& directory) {
    std::ofstream tarFile(tarFilePath, std::ios::binary);
    
    if (!tarFile.is_open()) {
        throw myerror();  // 如果 tar 文件无法打开，抛出 myerror 错误
    }

    try {
        // 创建 Tar 对象
        tarpp::Tar tar(tarFile);
        const std::time_t FIXED_TIME = 1703337600;
        // 递归遍历目录，并添加文件到 tar 文件
        for (auto& entry : fs::recursive_directory_iterator(directory)) {
            try {
                if (fs::is_regular_file(entry.status())) {
                    // 获取文件的最后修改时间
                    // std::time_t lastWriteTime = fs::last_write_time(entry.path());
                    std::time_t lastWriteTime = FIXED_TIME;
                    // 读取文件内容
                    std::ifstream file(entry.path().string(), std::ios::binary);
                    if (!file) {
                        throw myerror();
                    }

                    std::string fileContent((std::istreambuf_iterator<char>(file)),
                                            std::istreambuf_iterator<char>());
                    
                    // 获取相对路径（相对于顶级目录）
                    std::string relativePath = entry.path().string();
                    relativePath = relativePath.substr(directory.string().length() + 1);

                    // 使用 Boost 转换路径分隔符为正斜杠 '/'
                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

                    // 创建 TarFileOptions 可选项，并设置文件的时间戳
                    tarpp::TarFileOptions options(tarpp::details::DEFAULT_MODE(), 0, 0, lastWriteTime, tarpp::FileType::REGULAR, "", "", "");

                    // 将文件添加到 tar 中
                    tar.add(relativePath, fileContent, options);
                }
                else if (fs::is_directory(entry.status())) {
                    // 获取目录的最后修改时间
                    // std::time_t lastWriteTime = fs::last_write_time(entry.path());
                    std::time_t lastWriteTime = FIXED_TIME;
                    // 如果是目录，加入空文件夹（以保证目录结构存在）
                    std::string dirName = entry.path().string();
                    dirName = dirName.substr(directory.string().length() + 1);

                    // 使用 Boost 转换路径分隔符为正斜杠 '/'
                    std::replace(dirName.begin(), dirName.end(), '\\', '/');

                    // 目录需要以 '/' 结尾，并且在 tar 中要以 "目录" 类型保存
                    tar.add(dirName + "/", "", tarpp::TarFileOptions(tarpp::details::DEFAULT_MODE(), 0, 0, lastWriteTime, tarpp::FileType::DIRECTORY, "", "", ""));
                }
            }
            catch (const myerror& e) {
                std::cerr << "Error processing file: " << entry.path().string() << " - " << e.what() << std::endl;
            }
        }
        tarFile.close();
        // 完成 tar 文件的写入
        tar.finalize();
    }
    catch (const myerror& e) {
        std::cerr << "Error while creating tar: " << e.what() << std::endl;
        tarFile.close();
        throw;
    }

    // return tarFile;
}
// 将 Windows 风格的路径转换为 Linux 风格
// std::string normalize_path(const std::string& path) {
//     std::string normalized = path;
//     std::replace(normalized.begin(), normalized.end(), '\\', '/');
//     return normalized;
// }
// void createTar(const std::string& tar_path, const std::string& folder_path) {
//     struct archive* a = archive_write_new();
//     struct archive_entry* entry;
//     if (!a) {
//         throw std::runtime_error("无法创建archive对象");
//     }

//     archive_write_set_format_pax_restricted(a); // 设置tar格式
//     if (archive_write_open_filename(a, tar_path.c_str()) != ARCHIVE_OK) {
//         archive_write_free(a);
//         throw std::runtime_error("无法打开tar文件: " + tar_path);
//     }

//     fs::recursive_directory_iterator dir_iter(folder_path), end_iter;

//     for (; dir_iter != end_iter; ++dir_iter) {
//         const fs::path& file_path = dir_iter->path();
//         // std::string relative_path = fs::relative(file_path, folder_path).string();
//         std::string relative_path = normalize_path(fs::relative(file_path, folder_path).string());// 获取相对路径并转换为 Linux 风格
//         // 如果是目录，则创建一个目录条目
//         if (fs::is_directory(file_path)) {
//             entry = archive_entry_new();
//             archive_entry_set_pathname(entry, (relative_path + "/").c_str());  // 目录路径后要加 "/"
//             archive_entry_set_size(entry, 0); // 目录大小为0
//             archive_entry_set_filetype(entry, S_IFDIR); // 文件类型为目录
//             archive_entry_set_perm(entry, 0755); // 目录权限
//             archive_write_header(a, entry);
//             archive_entry_free(entry); // 释放目录条目
//         }
//         else if (fs::is_regular_file(file_path)) {
//             // 如果是文件，则创建一个文件条目
//             entry = archive_entry_new();
//             archive_entry_set_pathname(entry, relative_path.c_str());
//             archive_entry_set_size(entry, fs::file_size(file_path));
//             archive_entry_set_filetype(entry, S_IFREG);
//             archive_entry_set_perm(entry, 0644);

//             // 写入文件头
//             archive_write_header(a, entry);

//             // 读取文件并写入tar
//             std::ifstream file(file_path.string(), std::ios::binary);
//             char buffer[8192];
//             while (file.read(buffer, sizeof(buffer))) {
//                 archive_write_data(a, buffer, file.gcount());
//             }
//             if (file.gcount() > 0) {
//                 archive_write_data(a, buffer, file.gcount());
//             }

//             archive_entry_free(entry); // 释放文件条目
//         }
//     }

//     archive_write_close(a);
//     archive_write_free(a);
// }
/**
 * @brief 将tarFilePath解压到targetDirectory
 * 
 * @param tarFilePath 
 * @param targetDirectory 
 */
// void TarHeader::extractTar(const std::string& tarFilePath, const fs::path& targetDirectory){

// }
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