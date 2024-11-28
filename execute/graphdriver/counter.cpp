#include "filesys/graphdriver/counter.h"

// NewFsChecker 函数实现
std::shared_ptr<Checker_interface> NewFsChecker(FsMagic t) {
    return std::make_shared<FsChecker>(t);
}