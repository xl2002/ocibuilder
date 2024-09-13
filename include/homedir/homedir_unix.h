#if !defined(HOMEDIR_UNIX_H)
#define HOMEDIR_UNIX_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "unshare/unshare.h"
namespace homedir {
    std::string Key(){
        return "HOME";
    }
    std::string Get(){
        auto homedir=HomeDir();
        return homedir;
    }




}

#endif // HOMEDIR_UNIX_H)
