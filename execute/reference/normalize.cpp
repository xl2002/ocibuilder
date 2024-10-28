#include "reference/normalize.h"

const std::string 	
    legacyDefaultDomain = "index.docker.io",
	defaultDomain       = "docker.io",
	officialRepoName    = "library",
	defaultTag          = "latest";

std::pair<std::string, std::string> splitDockerDomain(const std::string& name);

std::shared_ptr<Named_interface> ParseNormalizedNamed(std::string s){
    auto split = splitDockerDomain(s);
    auto domain = split.first;
    auto remainder = split.second;
    std::string remoteName;
    size_t tagSep = remainder.find(':');
    if (tagSep != std::string::npos) {
        remoteName = remainder.substr(0, tagSep);
    } else {
        remoteName = remainder;
    }

    std::string lowerRemoteName = remoteName;
    std::transform(lowerRemoteName.begin(), lowerRemoteName.end(), lowerRemoteName.begin(), ::tolower);
    if (lowerRemoteName != remoteName) {
        throw std::invalid_argument("invalid reference format: repository name must be lowercase");
    }

    // 假设 Parse 是一个已定义的函数，用于解析引用
    auto  ref = Parse(domain + "/" + remainder);
    // 如果解析失败，抛出异常
    // std::shared_ptr<Named_interface> ref = Parse(domain + "/" + remainder); // 示例，实际代码中应该调用 Parse 函数

    if (!ref) { // 假设 Parse 函数返回 nullptr 表示失败
        throw std::runtime_error("reference " + (domain + "/" + remainder) + " has no name");
    }
    // auto n=std::make_shared<Named_interface>();
    // auto m=std::shared_ptr<Named_interface>(ref);
    auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    // if (!named) {
    //     throw std::runtime_error("reference has no name");
    // }
    // return std::shared_ptr<Named_interface>(named);
    // return ref; // 返回解析后的命名引用
    return named;
}

// splitDockerDomain 将仓库名称拆分为域名和剩余名称字符串。
// 如果未找到有效的域名，则使用默认域名。仓库名称
// 需要在此之前已经验证。
std::pair<std::string, std::string> splitDockerDomain(const std::string& name) {
    size_t i = name.find('/');
    std::string domain, remainder;

    if (i == std::string::npos || 
        (name.find_first_of(".:") == std::string::npos && name.substr(0, i) != "localhost")) {
        domain = defaultDomain;
        remainder = name;
    } else {
        domain = name.substr(0, i);
        remainder = name.substr(i + 1);
    }

    if (domain == legacyDefaultDomain) {
        domain = defaultDomain;
    }

    if (domain == defaultDomain && remainder.find('/') == std::string::npos) {
        remainder = officialRepoName + "/" + remainder;
    }

    return std::make_pair(domain, remainder);
}

std::shared_ptr<Named_interface> TagNameOnly(std::shared_ptr<Named_interface> named) {
    
    return named;
}