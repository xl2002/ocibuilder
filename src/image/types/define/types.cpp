#include "image/types/define/types.h"
#include "utils/common/go/string.h"

/**
 * @brief 检查并提取 URL 中的目录信息
 *
 * @param dir 目录路径
 * @param prefix URL 前缀
 * @param url 要处理的 URL
 * @param name 提取的名称，如果 URL 不是本地文件，则为空
 * @param subdir 提取的子目录，如果 URL 不是本地文件，则为空
 *
 * @return 无
 *
 * @throws 无
 */
void TempDirForURL(std::string dir,std::string prefix, std::string url,std::string& name, std::string& subdir){
    if (!hasPrefix(url, "http://") &&
        !hasPrefix(url, "https://") &&
        !hasPrefix(url, "git://") &&
        !hasPrefix(url, "github.com/") &&
        url != "-") {
            name = "";
            subdir = "";
    }
    return ;
}
/**
 * @brief 创建一个可选的布尔值
 *
 * @param b 布尔值
 * @return OptionalBool 可选的布尔值
 *
 * @throws 无
 */
OptionalBool NewOptionalBool(bool b){
    auto o=OptionalBool::OptionalBoolFalse;
    if(b){
        o=OptionalBool::OptionalBoolTrue;
    }
    return o;
}
