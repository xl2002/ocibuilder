#include "image/types/define/build.h"


/**
 * @brief 将Isolation枚举为字符串
 * 
 * @return string 
 */
string Isolation::String(){
    if(value==IsolationDefault||value==IsolationOCI){
        return "oci";
    }else if(value==IsolationChroot){
        return "chroot";
    }else if(value==IsolationOCIRootless){
        return "rootless";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}

/**
 * @brief 将Compression枚举为字符串
 * 
 * @return string 返回对应的压缩文件扩展名
 */
string Compression::String(){
    if(value==Uncompressed){
        return tarExt;
    }else if(value==Bzip2){
        return tarExt+".bz2";
    }else if(value==Gzip){
        return tarExt+".gz";
    }else if(value==Xz){
        return tarExt+".xz";
    }else if(value==Zstd){
        return tarExt+".zst";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}
/**
 * @brief 将NetworkConfigurationPolicy枚举转换为字符串
 * 
 * @return string 返回对应的字符串表示
 * 
 * - NetworkDefault: 默认网络配置
 * - NetworkDisabled: 禁用网络
 * - NetworkEnabled: 启用网络
 */
string NetworkConfigurationPolicy::String(){
    if(value==NetworkDefault){
        return "NetworkDefault";
    }else if(value==NetworkDisabled){
        return "NetworkDisabled";
    }else if(value==NetworkEnabled){
        return "NetworkEnabled";
    }else{
        std::cerr<<"unknown NetworkConfigurationPolicy "<<value<<std::endl;
        return "";
    }
}
