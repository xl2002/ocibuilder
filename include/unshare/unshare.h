#if !defined(UNSHARE_UNSHARE_H)
#define UNSHARE_UNSHARE_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>

std::string HomeDir(){
    #if defined(__linux__)
        return std::string("/root");
    #else
        return std::string("D:\\oci_images");
    #endif
}

#endif // UNSHARE_UNSHARE_H)q
