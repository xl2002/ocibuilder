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
/**
 * @brief 将两个路径字符串连接在一起，确保路径之间没有多余的分隔符。
 * 
 * @param path1 第一个路径字符串
 * @param path2 第二个路径字符串
 * @return 连接后的路径字符串
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
 * @return 一个环境变量的向量。
 */
std::vector<std::string> LookupEnvVarReferences(const std::vector<std::string>& specs) {
    std::vector<std::string> result;
    result.reserve(specs.size());

    for (const auto& spec : specs) {
        size_t pos = spec.find('=');
        if (pos != std::string::npos) {
            // If the spec has an '=', it's a literal env var assignment.
            result.push_back(spec);

        } else if (spec == "*") {
            // If the spec is '*', use all environment variables.
            // result.insert(result.end(), environ.begin(), environ.end());

        } else {
            std::string prefix = spec + "=";
            if (spec.back() == '*') {
                // If the spec ends with '*', strip it off for the prefix.
                prefix.pop_back();
            }

            // for (const auto& env : environ) {
            //     if (env.compare(0, prefix.size(), prefix) == 0) {
            //         // If the env var matches the prefix, add it to the result.
            //         result.push_back(env);
            //     }
            // }
        }
    }

    return result;
}
