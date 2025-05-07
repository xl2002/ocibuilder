#include "filesys/graphdriver/counter.h"

/**
 * @brief 创建一个新的文件系统检查器
 * @param t 文件系统类型标识 
 * @return 返回文件系统检查器接口指针
 */
std::shared_ptr<Checker_interface> NewFsChecker(FsMagic t) {
    return std::make_shared<FsChecker>(t);
}
