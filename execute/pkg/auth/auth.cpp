#include "pkg/auth/auth.h"
/**
 * @brief CheckAuthFile 该函数用来判断给定的文件路径是否可以访问
 * @param pathOption 该参数是要检查的文件路径
 * @throws myerror 如果文件路径是无效的，或者无法访问该文件
 */
void CheckAuthFile(std::string pathOption){
    if(pathOption.empty()){
        return;
    }
    struct stat buffer;
    if(stat(pathOption.c_str(),&buffer)!=0){
        throw myerror("credential file is not accessible:"+std::make_error_code(std::errc::no_such_file_or_directory).message());
    }
}


