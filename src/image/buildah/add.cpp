#include "image/buildah/buildah.h"
/**
 * @brief 将destination的内容添加到mountPoint的diff文件夹中
 * 
 * @param destination 
 * @param extract 
 * @param options 
 * @param sources 
 */
void Builder::Add(std::string destination,bool extract,std::shared_ptr<AddAndCopyOptions> options,std::vector<std::string> sources){
//这个函数复杂，简单的按照文件复制处理

}