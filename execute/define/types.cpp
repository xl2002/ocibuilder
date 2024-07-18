#include "define/types.h"

bool hasPrefix(const std::string& str, const std::string& prefix) {
    return str.compare(0, prefix.length(), prefix) == 0;
}
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