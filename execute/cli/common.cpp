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
/**
 * @brief 要围绕分隔符的第一个实例进行分割.
 * 
 * @param str 
 * @param delimiter 
 * @param max_splits 
 * @return vector<string> 
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

store* getStore(Command* cmd){
    return new store();
}

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
std::string JoinPath(const std::string& path1, const std::string& path2) {
    char sep = '/';
    std::string result = path1;

    // Ensure the first path does not end with a separator
    if (!result.empty() && result[result.size() - 1] == sep) {
        result.pop_back();
    }

    // Ensure the second path does not start with a separator
    if (!path2.empty() && path2[0] == sep) {
        result += path2;
    } else {
        result += sep + path2;
    }

    return result;
}
