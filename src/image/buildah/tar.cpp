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
 * @brief 根据tarfilePath和directory创建tarFilterer
 *  
 * @param tarFilePath 
 * @param directory 
 * @return std::shared_ptr<tarFilterer> 
 */

std::shared_ptr<tarFilterer> newTarFilterer(const std::string& tarFilePath,const fs::path& directory){
    
}
/**
 * @brief 关闭pipeWriter_TarHeader文件流
 * 
 */
void  tarFilterer::Close(){

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
std::shared_ptr<digester_interface> newTarDigester(string contentType){

}