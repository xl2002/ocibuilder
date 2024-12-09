#include "image/buildah/tar.h"


namespace fs = boost::filesystem;

/**
 * @brief 创建新tar文件，将directory下的文件写入,返回文件流
 * 
 * @param tarFilePath 
 * @param directory 
 * @return std::ofstream 
 */
std::ofstream createTar(const std::string& tarFilePath, const fs::path& directory) {
    std::ofstream tarFile(tarFilePath, std::ios::binary);
    
    if (!tarFile.is_open()) {
        throw myerror();  // 如果 tar 文件无法打开，抛出 myerror 错误
    }

    try {
        // 创建 Tar 对象
        tarpp::Tar tar(tarFile);

        // 递归遍历目录，并添加文件到 tar 文件
        for (auto& entry : fs::recursive_directory_iterator(directory)) {
            try {
                if (fs::is_regular_file(entry.status())) {
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

                    // 创建 TarFileOptions 可选项
                    tarpp::TarFileOptions options;
                    
                    // 将文件添加到 tar 中
                    tar.add(relativePath, fileContent, options);
                }
                else if (fs::is_directory(entry.status())) {
                    // 如果是目录，加入空文件夹（以保证目录结构存在）
                    std::string dirName = entry.path().string();
                    dirName = dirName.substr(directory.string().length() + 1);
                    tar.add(dirName + "/", "", tarpp::TarFileOptions());
                }
            }
            catch (const myerror& e) {
                std::cerr << "Error processing file: " << entry.path().string() << " - " << e.what() << std::endl;
            }
        }

        // 完成 tar 文件的写入
        tar.finalize();
    }
    catch (const myerror& e) {
        std::cerr << "Error while creating tar: " << e.what() << std::endl;
        tarFile.close();
        throw;
    }

    return tarFile;
}
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
    // 创建tarFilterer对象
    auto filterer = std::make_shared<tarFilterer>();

    // 打开输出文件流用于写tar文件
    filterer->pipeWriter_TarHeader.open(tarFilePath, std::ios::binary);
    if (!filterer->pipeWriter_TarHeader.is_open()) {
        throw std::runtime_error("无法打开文件: " + tarFilePath);
    }

    // 创建tarWriter对象
    tarpp::Tar tarWriter(filterer->pipeWriter_TarHeader);

    // 使用Boost文件系统递归遍历目录
    for (fs::recursive_directory_iterator it(directory), end; it != end; ++it) {
        const fs::path& filePath = *it;
        
        if (fs::is_directory(filePath)) {
            // 跳过目录
            continue;
        }

        // 为每个文件创建TarHeader
        tarpp::details::TarHeader header;
        tarpp::details::format_name(header, filePath.filename().string());  // 格式化文件名
        tarpp::format::format_octal(header.header_.mode_, 0644);
        tarpp::format::format_octal(header.header_.uid_, 1000);
        tarpp::format::format_octal(header.header_.gid_, 1000);
        tarpp::format::format_octal_no_null(header.header_.size_, fs::file_size(filePath));
        tarpp::format::format_octal_no_null(header.header_.mtime_, 0);
        header.header_.type_[0] = '0';  // 常规文件
        tarpp::format::format_string_opt_null(header.header_.linkname_, "");
        tarpp::format::format_string(header.header_.uname_, "user");
        tarpp::format::format_string(header.header_.gname_, "group");

        // 设置校验和
        tarWriter.set_checksum(header);

        // 将Header写入tar文件
        tarWriter.add(filePath.filename().string(), "", tarpp::TarFileOptions{});

        // 处理文件内容（如果有）
        if (header.header_.size_ != 0) {
            std::ifstream file(filePath.string(), std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("无法打开文件: " + filePath.string());
            }

            std::string fileContent((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
            tarWriter.add(filePath.filename().string(), fileContent);
        }
    }

    // 返回创建的tarFilterer对象
    return filterer;
}




// 实现newTarFilterer函数
// std::shared_ptr<tarFilterer> newTarFilterer(
//     std::ostream& writeStream,  // 写入流
//     std::function<std::tuple<bool, bool, std::istream*>(std::shared_ptr<tarpp::details::TarHeader>)> filter  // 过滤器
// ) {
//     // 创建过滤器对象
//     std::shared_ptr<tarFilterer> filterer = std::make_shared<tarFilterer>();
//         // 使用标准输入流（std::cin）直接读取
//     std::istream& inputStream = std::cin;

//     // 使用boost线程库启动线程
//     filterer->wg.create_thread([&filterer, &writeStream, filter]() {
//         try {

//             tarpp::Tar tarWriter(writeStream);

//             while (true) {
//                 tarpp::details::TarHeader header;
//                 inputStream.read(reinterpret_cast<char*>(&header), sizeof(header));
//                 if (inputStream.gcount() <= 0) break;  // 读取结束

//                 bool skip = false;
//                 bool replaceContents = false;
//                 std::istream* replacementContents = nullptr;

//                 // 调用过滤器进行处理
//                 if (filter) {
//                     std::tie(skip, replaceContents, replacementContents) = filter(std::make_shared<tarpp::details::TarHeader>(header));
//                 }

//                 // 如果不跳过，写入TarHeader和内容
//                 if (!skip) {
//                     tarWriter.add(header.header_.name_, std::string(), tarpp::TarFileOptions{}); // 使用TarFileOptions 默认值
//                     if (header.header_.size_ != 0) {
//                         if (replaceContents) {
//                             // 通过替换内容写入
//                             tarWriter.add(header.header_.name_, std::string(), tarpp::TarFileOptions{}); // 假设替换为其他内容
//                         } else {
//                             // 普通内容写入
//                             tarWriter.add(header.header_.name_, std::string(), tarpp::TarFileOptions{}); 
//                         }
//                     }
//                 }
//             }

//             // 完成后关闭所有流
//             tarWriter.finalize();
//             filterer->Close();
//         } catch (const myerror& e) {
//             std::cerr << "Error: " << e.what() << std::endl;
//         }
//     });
//     return filterer;
// }

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
void tarDigester::write(const std::string& data){
    // this->nested->write(data);
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
std::shared_ptr<digester_interface> newTarDigester(const std::string& contentType, const std::string& tarFilePath, const fs::path& directory) {
    // 创建嵌套的 digester
    auto nested = newSimpleDigester(contentType);

    // 创建 tarFilterer 对象并配置它，传入 tarFilePath 和 directory
    auto tarFilterer = newTarFilterer(tarFilePath, directory);

    // 创建并初始化 tarDigester 对象
    auto digester = std::make_shared<tarDigester>();
    digester->isOpen = true;
    digester->nested = nested;
    digester->tarFilterer = tarFilterer;

    return digester;
}