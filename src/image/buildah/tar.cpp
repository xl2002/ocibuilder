#include "image/buildah/tar.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace fs = boost::filesystem;

/**
 * @brief 创建新tar文件，将directory下的文件写入,返回文件流
 * 
 * @param tarFilePath 
 * @param directory 
 * @return std::ofstream 
 */
void createTar(const std::string& tarFilePath, const fs::path& directory) {
    std::chrono::duration<double> duration;
    std::chrono::high_resolution_clock::time_point start,end;
    start=std::chrono::high_resolution_clock::now();
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
                    // 读取文件内容
                    std::ifstream file(entry.path().string(), std::ios::binary);
                    if (!file) {
                        throw myerror();
                    }

                    std::string fileContent((std::istreambuf_iterator<char>(file)),
                                            std::istreambuf_iterator<char>());
                    file.close();
                    // 获取相对路径（相对于顶级目录）
                    std::string relativePath = entry.path().string();
                    relativePath = relativePath.substr(directory.string().length() + 1);

                    // 使用 Boost 转换路径分隔符为正斜杠 '/'
                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

                    // 将文件添加到 tar 中（使用默认 TarFileOptions）
                    tar.add(relativePath, fileContent);
                }
                else if (fs::is_directory(entry.status())) {
                    // 如果是目录，加入空文件夹（以保证目录结构存在）
                    std::string dirName = entry.path().string();
                    dirName = dirName.substr(directory.string().length() + 1);

                    // 使用 Boost 转换路径分隔符为正斜杠 '/'
                    std::replace(dirName.begin(), dirName.end(), '\\', '/');

                    // 目录需要以 '/' 结尾，并且在 tar 中要以 "目录" 类型保存
                    tar.add(dirName + "/", "");
                }
            }
            catch (const myerror& e) {
                std::cerr << "Error processing file: " << entry.path().string() << " - " << e.what() << std::endl;
            }
        }
        tarFile.close();
        // 完成 tar 文件的写入
        tar.finalize();
        end=std::chrono::high_resolution_clock::now();
        duration=end-start;
        std::cout<<"createTar from "<<directory.string()<<" duration: "<<duration.count()<<" s"<<std::endl;
    }
    catch (const myerror& e) {
        std::cerr << "Error while creating tar: " << e.what() << std::endl;
        tarFile.close();
        throw;
    }

    // return tarFile;
}
//多线程版本
// void createTar(const std::string& tarFilePath, const fs::path& directory) {
//     // std::chrono::duration<double> duration;
//     // std::chrono::high_resolution_clock::time_point start,end;
//     // start=std::chrono::high_resolution_clock::now();

//     std::ofstream tarFile(tarFilePath, std::ios::binary);
//     if (!tarFile.is_open()) {
//         throw myerror();  // 如果 tar 文件无法打开，抛出 myerror 错误
//     }

//     try {
//         // 创建 Tar 对象
//         tarpp::Tar tar(tarFile);
//         const std::time_t FIXED_TIME = 1703337600;

//         // 使用 Boost 线程池来处理文件和目录的读取
//         boost::asio::thread_pool pool;

//         // 递归遍历目录，并将文件添加到 tar 文件
//         std::vector<std::string> directoriesToAdd;
//         std::vector<std::string> filesToAdd;

//         // 收集所有文件和目录信息，避免在多线程中频繁访问文件系统
//         for (auto& entry : fs::recursive_directory_iterator(directory)) {
//             if (fs::is_regular_file(entry.status())) {
//                 filesToAdd.push_back(entry.path().string());
//             } else if (fs::is_directory(entry.status())) {
//                 directoriesToAdd.push_back(entry.path().string());
//             }
//         }

//         // 并行处理文件
//         for (auto& filePath : filesToAdd) {
//             boost::asio::post(pool, [&, filePath]() {
//                 try {
//                     std::ifstream file(filePath, std::ios::binary);
//                     if (!file) {
//                         throw myerror();
//                     }

//                     std::string fileContent((std::istreambuf_iterator<char>(file)),
//                                             std::istreambuf_iterator<char>());
//                     file.close();

//                     // 获取相对路径（相对于顶级目录）
//                     std::string relativePath = filePath;
//                     relativePath = relativePath.substr(directory.string().length() + 1);

//                     // 使用 Boost 转换路径分隔符为正斜杠 '/'
//                     std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

//                     // 将文件添加到 tar 中
//                     tar.add(relativePath, fileContent);
//                 }
//                 catch (const myerror& e) {
//                     std::cerr << "Error processing file: " << filePath << " - " << e.what() << std::endl;
//                 }
//             });
//         }

//         // 并行处理目录
//         for (auto& dirPath : directoriesToAdd) {
//             boost::asio::post(pool, [&, dirPath]() {
//                 try {
//                     std::string dirName = dirPath;
//                     dirName = dirName.substr(directory.string().length() + 1);

//                     // 使用 Boost 转换路径分隔符为正斜杠 '/'
//                     std::replace(dirName.begin(), dirName.end(), '\\', '/');

//                     // 目录需要以 '/' 结尾，并且在 tar 中要以 "目录" 类型保存
//                     tar.add(dirName + "/", "");
//                 }
//                 catch (const myerror& e) {
//                     std::cerr << "Error processing directory: " << dirPath << " - " << e.what() << std::endl;
//                 }
//             });
//         }

//         // 等待所有任务完成
//         pool.join();

//         // 完成 tar 文件的写入
//         tar.finalize();
//         tarFile.close();

//         // end=std::chrono::high_resolution_clock::now();
//         // duration=end-start;
//         // std::cout<<"createTar from "<<directory.string()<<" duration: "<<duration.count()<<" s"<<std::endl;
//     }
//     catch (const myerror& e) {
//         std::cerr << "Error while creating tar: " << e.what() << std::endl;
//         tarFile.close();
//         throw;
//     }
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