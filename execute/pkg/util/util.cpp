#include "pkg/util/util.h"
#include <windows.h>
#include <sys/stat.h>
#include <vector>
bool MirrorToTempFileIfPathIsDescriptor(std::string file ,const std::string &authfile){
    if(file.substr(0,8)!="/dev/fd/"){
        return false;
    }
    try
    {
        std::ifstream infile(file,std::ios::binary);
        if(!infile.is_open()){
            throw myerror("Unable to open file: " + file);
        }
        infile.close();
    }
    catch(const myerror & e)
    {
        throw;
    }
}

std::string joinPath(const std::string& path1, const std::string& path2) {
    if (path1.back() == '\\') {
        return path1 + path2;
    }
    return path1 + "\\" + path2;
}

bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool isDirectory(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return (buffer.st_mode & S_IFDIR) != 0;
}

bool isRegularFile(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return (buffer.st_mode & S_IFREG) != 0;
}

std::string DiscoverContainerfile(std::string path){
    struct stat target;
    std::string foundCtrFile;
    if(stat(path.c_str(),&target)!=0){
        throw myerror("discovering Containerfile: " + std::string(strerror(errno)));
    }
    if((target.st_mode& S_IFDIR)!=0){
        std::string ctrfile = joinPath(path, "Containerfile");
        if (!fileExists(ctrfile)) {
            ctrfile = joinPath(path, "Dockerfile");
            if (!fileExists(ctrfile)) {
                throw myerror("cannot find Containerfile or Dockerfile in context directory");
            }
        }

        if (isRegularFile(ctrfile)) {
            foundCtrFile = ctrfile;
        } else {
            throw myerror("assumed Containerfile " + ctrfile + " is not a file");
        }
    } else if ((target.st_mode & S_IFREG) != 0) {
        foundCtrFile = path;
    } else {
        throw myerror("path is neither a regular file nor a directory");
    }
    return foundCtrFile;
}

std::string getDirectory(const std::string& path) {
    size_t pos = path.find_last_of("\\/");
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(0, pos);
}
std::string resolveSymlinks(const std::string& path) {
    std::vector<char> buffer(MAX_PATH);
    DWORD result = GetFullPathNameA(path.c_str(), MAX_PATH, buffer.data(), nullptr);
    if (result == 0) {
        throw std::runtime_error("Error resolving symlinks: " + std::to_string(GetLastError()));
    }
    return std::string(buffer.data());
}