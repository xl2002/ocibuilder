#if !defined(FILESYS_UTILS_HOMEDIR_UNIX_H)
#define FILESYS_UTILS_HOMEDIR_UNIX_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "filesys/system/unshare.h"
namespace homedir {
    std::string Key();
    std::string Get();
}

#endif // HOMEDIR_UNIX_H)
