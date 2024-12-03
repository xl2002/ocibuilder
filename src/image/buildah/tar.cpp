#include "image/buildah/tar.h"
/**
 * @brief 创建新tar文件，将directory下的文件写入,返回文件流
 * 
 * @param tarFilePath 
 * @param directory 
 * @return std::ofstream 
 */
std::ofstream TarHeader::createTar(const std::string& tarFilePath, const fs::path& directory){
    return std::ofstream(tarFilePath, std::ios::out | std::ios::binary);
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