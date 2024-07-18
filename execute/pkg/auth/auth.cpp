#include "pkg/auth/auth.h"
void CheckAuthFile(std::string pathOption){
    if(pathOption.empty()){
        return;
    }
    struct stat buffer;
    if(stat(pathOption.c_str(),&buffer)!=0){
        throw myerror("credential file is not accessible:"+std::make_error_code(std::errc::no_such_file_or_directory).message());
    }
}


