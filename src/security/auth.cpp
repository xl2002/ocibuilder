#include "security/auth.h"
/**
 * @brief CheckAuthFile 该函数用来判断给定的文件路径是否可以访问
 * @param pathOption 该参数是要检查的文件路径
 * @throws myerror 如果文件路径是无效的，或者无法访问该文件
 */
void CheckAuthFile(pushOptions* iopts){
    if(iopts->authfile.empty()){
        //如果没有认真令牌，发送网络请求获得令牌
        return;
    }
    struct stat buffer;
    if(stat(iopts->authfile.c_str(),&buffer)!=0){
        throw myerror("credential file is not accessible:"+std::make_error_code(std::errc::no_such_file_or_directory).message());
    }
}


