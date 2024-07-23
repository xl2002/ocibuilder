/**
 * @file common.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 对common.h头文件中的声明进行定义
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "cli/common.h"
#include "define/types.h"
#include <cstdlib>
#include <algorithm>
#include <windows.h>
/**
 * @brief 根据分隔符对字符串进行拆分，最多拆分max_splits-1次，返回一个包含拆分结果的vector<string>。
 * 
 * @param str 要进行拆分的字符串
 * @param delimiter 拆分字符串的分隔符
 * @param max_splits 最多拆分次数，默认为-1，表示拆分所有实例
 * @return vector<string> 一个包含拆分结果的string向量
 */
vector<string> SplitN(const string& str, const string& delimiter, size_t max_splits) {
    vector<string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    size_t splits = 0;

    while (end != string::npos && splits < max_splits - 1) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
        ++splits;
    }
    result.push_back(str.substr(start)); // 添加字符串的剩余部分

    return result;
}
/**
 * @brief 将字符串解析成布尔类型
 * 将字符串解析成布尔类型，字符串的值可能是 true, false, 1, 0, t, f
 * 
 * @param str 
 * @return string 
 */
string ParseBool(string str){
    string lower_str = str;
    transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);

    if (lower_str == "1" || lower_str == "t" || lower_str == "true") {
        return "true";
    }
    if (lower_str == "0" || lower_str == "f" || lower_str == "false") {
        return "false";
    }
    return "false";
}

/**
 * @brief 根据命令 cmd 获取对应的 store 对象
 * @param cmd 命令
 * @return shared_ptr<store> store 对象
 */
shared_ptr<store> getStore(Command* cmd){
    auto needToShutdownStore =false;
    // if (!setXDGRuntimeDir()) {
    //     cerr << "Failed to set XDG runtime directory" << endl;
    //     return nullptr;
    // }
    //todo

    StoreOptions options = DefaultStoreOptions();
    
    // if (cmd->Flag_find("root")->changed || cmd->Flag_find("runroot")->changed) {    
    // }

    if (cmd->Flag_find("storage-driver")->changed) {
    }

    if (cmd->Flag_find("storage-opt")->changed) {
    }

    // if (geteuid() != 0 && options.graph_driver_name != "vfs") {
    //     cerr << "Cannot mount using driver " << options.graph_driver_name << " in rootless mode. You need to run it in a `buildah unshare` session" << endl;
    //     return nullptr;
    // }
    ///<检查用户是否为root且存储引擎设置是否为vfs

    // if (!globalFlagOptions.UserNSUID.empty()) {
    //     vector<string> uopts = globalFlagResults.UserNSUID;
    //     vector<string> gopts = globalFlagResults.UserNSGID;

    //     if (gopts.empty()) {
    //         gopts = uopts;
    //     }

    //     try {
    //         tie(options.UIDMap, options.GIDMap) = ParseIDMappings(uopts, gopts);
    //     } catch (const exception& e) {
    //         cerr << e.what() << endl;
    //         return nullptr;
    //     }
    // } else if (!globalFlagResults.UserNSGID.empty()) {
    //     cerr << "option --userns-gid-map cannot be used without --userns-uid-map" << endl;
    //     return nullptr;
    // }
    ///<以上注释代码处理用户命名空间映射，用户命名空间映射的目的和意义：
    ///<用户命名空间（User Namespace）是Linux内核的一项功能，
    ///<它允许进程在容器或隔离环境中拥有不同于宿主系统的用户和组ID（UID和GID）。
    ///<这意味着在容器内，进程可以运行为“root”，而在宿主系统上它们的UID和GID实际上是非特权用户的ID。

    // if (cmd->Flags()->Lookup("userns-uid-map")->changed) {
    //     vector<string> uopts = cmd->GetStringSlice("userns-uid-map");
    //     vector<string> gopts = cmd->GetStringSlice("userns-gid-map");

    //     if (gopts.empty()) {
    //         gopts = uopts;
    //     }

    //     try {
    //         tie(options.UIDMap, options.GIDMap) = ParseIDMappings(uopts, gopts);
    //     } catch (const exception& e) {
    //         cerr << e.what() << endl;
    //         return nullptr;
    //     }
    // } else if (cmd->Flags()->Lookup("userns-gid-map")->changed) {
    //     cerr << "option --userns-gid-map cannot be used without --userns-uid-map" << endl;
    //     return nullptr;
    // }
    ///<这段代码与上面处理用户命名空间原理相同，只不过是处理CMD下面的子命令如何使用

    umask(0);

    shared_ptr<store> store = GetStore(options);
    if (store) {
        // 假设 is::Transport::SetStore(store) 的替代实现
        // is.Transport.SetStore(store);
    }
    needToShutdownStore = true;
    return store;
}

/**
 * @brief 根据镜像类型获取对应的 manifest 类型
 * 
 * @param format 镜像类型
 * @return string manifest 类型
 * @throws myerrormirror image type
 */
string GetFormat(string format){
    if(format==OCI){
        return OCIv1ImageManifest;
    }else if(format==DOCKER){
        return Dockerv2ImageManifest;
    }
    else{
        throw myerror("unrecognized image type "+format);
    }
}

bool UseLayers(){
    const char* layersEnv=std::getenv("BUILDAH_LAYERS");
    if (!layersEnv) {
        return false;
    }
    std::string layers = layersEnv;
    std::transform(layers.begin(), layers.end(), layers.begin(), ::tolower);
    return layers == "true" || layers == "1";
}
/**
 * @brief 将两个路径字符串连接在一起，确保路径之间没有多余的分隔符。
 * 
 * @param path1 第一个路径字符串
 * @param path2 第二个路径字符串
 * @return 连接后的路径字符串
 * 
 * 该函数将两个路径字符串连接在一起，确保路径之间没有多余的分隔符。
 * 例如，JoinPath("a", "b") 将返回 "a/b"，而不是 "a//b"。
 */
std::string JoinPath(const std::string& path1, const std::string& path2) {
    char sep = '/'; // 使用 '/' 作为分隔符
    std::string result = path1;

    // 确保第一个路径字符串不以分隔符结尾
    if (!result.empty() && result[result.size() - 1] == sep) {
        result.pop_back();
    }

    // 确保第二个路径字符串不以分隔符开头
    if (!path2.empty() && path2[0] == sep) {
        result += path2;
    } else {
        result += sep + path2;
    }

    return result;
}


/**
 * @brief DecryptConfigs 根据解密密钥构造一个 DecryptConfig 对象。
 * @param decryptionKeys 解密密钥的向量。
 * @return 一个指向 DecryptConfig 对象的 shared_ptr，如果解密密钥向量为空，则返回一个空的 shared_ptr。
 */
shared_ptr<DecryptConfig> DecryptConfigs(vector<string> decryptionKeys) {
    auto decryptConfig = make_shared<DecryptConfig>();
    if (decryptionKeys.size() > 0) {
        // 如果解密密钥向量不为空，则可以对 DecryptConfig 对象进行初始化操作
    }
    return decryptConfig;
}
/**
 * @brief LookupEnvVarReferences 从 specs 中查找环境变量。
 * @param specs 一个环境变量规格的向量。
 * 
 * specs 中的每个元素可以是环境变量的规格，规格有两种形式：
 * 1. 环境变量的名称 + "=" + 环境变量的值，例如："FOO=bar"。
 * 2. 环境变量的前缀，例如："FOO="。
 * 
 * 如果 specs 中的元素是规格 1，则该元素会被添加到结果向量中。
 * 如果 specs 中的元素是规格 2，并且环境中存在以该前缀开头的环境变量，
 * 则以该前缀开头的所有环境变量都会被添加到结果向量中。
 * 
 * 如果 specs 中的元素是 "*"，则所有的环境变量都会被添加到结果向量中。
 * 
 * @return 一个环境变量的向量。
 */
std::vector<std::string> LookupEnvVarReferences(const std::vector<std::string>& specs) {
    std::vector<std::string> result;
    result.reserve(specs.size());

    for (const auto& spec : specs) {
        size_t pos = spec.find('=');
        if (pos != std::string::npos) {
            // 如果 spec 有 '='，则是一个环境变量的规格，添加到结果向量中。
            result.push_back(spec);

        } else if (spec == "*") {
            // 如果 spec 是 '*'，则使用所有的环境变量。
            // result.insert(result.end(), environ.begin(), environ.end());

        } else {
            std::string prefix = spec + "=";
            if (spec.back() == '*') {
                // 如果 spec 以 '*' 结尾，则去掉 '*' 作为前缀。
                prefix.pop_back();
            }

            // for (const auto& env : environ) {
            //     if (env.compare(0, prefix.size(), prefix) == 0) {
            //         // 如果 env 以 prefix 开头，则添加到结果向量中。
            //         result.push_back(env);
            //     }
            // }
        }
    }

    return result;
}
/**
 * @brief 获取给定路径的绝对路径
 * 
 * 该函数使用 C 函数 _fullpath 来获取给定路径的绝对路径。
 * 如果获取绝对路径失败，会抛出一个 myerror 异常。
 * 
 * @param path 给定的路径
 * @return 绝对路径
 * @throws myerror 如果获取绝对路径失败
 */
string Abspath(string path){
    // 定义一个大小为 MAX_PATH 的字符数组，用于存储绝对路径
    char fullPath[MAX_PATH];
    // 使用 _fullpath 函数来获取给定路径的绝对路径，并将结果存储在 fullPath 中
    // 如果获取绝对路径失败，_fullpath 返回 nullptr
    if(_fullpath(fullPath,path.c_str(),MAX_PATH)==nullptr){
        // 如果获取绝对路径失败，抛出一个 myerror 异常，并提供错误信息
        throw myerror ("Failed to obtain the absolute path. ");
    }
    // 返回绝对路径
    return string(fullPath);
}

