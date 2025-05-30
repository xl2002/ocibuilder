#include "image/types/reference/normalize.h"
#include "utils/logger/ProcessSafeLogger.h"

std::pair<std::string, std::string> splitDockerDomain(const std::string& name);

/**
 * @brief 解析并规范化命名引用
 * 
 * @param s 要解析的字符串引用
 * @return std::shared_ptr<Named_interface> 返回解析后的命名接口指针
 * @throws std::invalid_argument 如果仓库名称不是小写
 * @throws std::runtime_error 如果引用没有名称
 */
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
        logger->log_error("invalid reference format: repository name must be lowercase");
        throw std::invalid_argument("invalid reference format: repository name must be lowercase");
    }

    // 假设 Parse 是一个已定义的函数，用于解析引用
    auto  ref = Parse(domain + "/" + remainder);
    // 如果解析失败，抛出异常
    // std::shared_ptr<Named_interface> ref = Parse(domain + "/" + remainder); // 示例，实际代码中应该调用 Parse 函数

    if (!ref) { // 假设 Parse 函数返回 nullptr 表示失败
        logger->log_error("reference " + (domain + "/" + remainder) + " has no name");
        throw std::runtime_error("reference " + (domain + "/" + remainder) + " has no name");
    }
    // auto n=std::make_shared<Named_interface>();
    // auto m=std::shared_ptr<Named_interface>(ref);
    // auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    auto named = std::dynamic_pointer_cast<Named_interface>(ref);
    // if (!named) {
    //     throw std::runtime_error("reference has no name");
    // }
    // return std::shared_ptr<Named_interface>(named);
    // return ref; // 返回解析后的命名引用
    // return ref;
    return named; // 返回解析后的命名引用
}

// splitDockerDomain 将仓库名称拆分为域名和剩余名称字符串。
// 如果未找到有效的域名，则使用默认域名。仓库名称
// 需要在此之前已经验证。
/**
 * @brief 拆分Docker域名和剩余名称
 * 
 * @param name 完整的仓库名称
 * @return std::pair<std::string, std::string> 返回域名和剩余名称对
 */
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


/**
 * @brief 为命名引用添加标签
 * 
 * @param name 命名接口指针
 * @param tag 要添加的标签
 * @return std::shared_ptr<NamedTagged_interface> 返回带标签的命名接口指针
 * @throws myerror 如果标签格式无效
 */
std::shared_ptr<NamedTagged_interface> WithTag(std::shared_ptr<Named_interface> name, const std::string& tag) {
    if (!std::regex_match(tag, *(anchoredTagRegexp->GetRegex()))) {
        logger->log_error("标签格式无效");
        throw myerror("标签格式无效");
    }

    std::shared_ptr<repository> repo = std::make_shared<repository>();

    // 尝试将 name 转换为 namedRepository_interface
    if (auto r = std::dynamic_pointer_cast<namedRepository_interface>(name)) {
        repo->domain = r->Domain();
        repo->path = r->Path();
    } else {
        // 如果不是 namedRepository_interface，使用 name 的 Name() 方法
        repo->path = name->Name();
    }

    if (auto canonical = std::dynamic_pointer_cast<Canonical_interface>(name)) {
        // return std::make_shared<reference>(
        //     repo, // namedRepository
        //     tag, // tag
        //     std::make_shared<Digest>(canonical->Digests()) // 假设这里可以直接用指针
        // );
    }
    auto ret=std::make_shared<taggedReference>(repo,tag);
    auto n=std::dynamic_pointer_cast<NamedTagged_interface>(ret);
    if(n==nullptr){
        logger->log_error("WithTag fail");
        std::cerr<<"WithTag fail"<<std::endl;
    }
    return n;
}

// TagNameOnly 如果引用仅具有仓库名称，则将默认标签“latest”添加到引用中。
/**
 * @brief 为仅包含名称的引用添加默认标签
 * 
 * @param ref 命名接口指针
 * @return std::shared_ptr<Named_interface> 返回带默认标签的命名接口指针
 * @throws myerror 如果添加标签失败
 */
std::shared_ptr<Named_interface> TagNameOnly(const std::shared_ptr<Named_interface>& ref) {
    if (IsNameOnly(ref)) {
        std::shared_ptr<NamedTagged_interface> namedTagged;
        try {
            namedTagged = WithTag(ref, defaultTag);
        } catch (const myerror& e) {
            throw; // 重新抛出异常
        }
        return namedTagged;
    }
    return ref;
}


// ParseDockerRef 规范化图像引用，遵循Docker约定。主要是为了向后兼容。
// 返回的引用只能是标记或摘要形式。如果引用包含标签和摘要，
// 函数将返回摘要引用。
/**
 * @brief 解析Docker引用并规范化
 * 
 * @param ref 要解析的Docker引用字符串
 * @return std::shared_ptr<Named_interface> 返回规范化后的命名接口指针
 * @throws myerror 如果解析失败
 */
std::shared_ptr<Named_interface> ParseDockerRef(const std::string& ref) {
    try {
        std::shared_ptr<Named_interface> named = ParseNormalizedNamed(ref);
        if (!named) {
            logger->log_error("解析规范化命名失败");
            throw myerror("解析规范化命名失败");
        }

        if (std::shared_ptr<NamedTagged_interface> tagged = std::dynamic_pointer_cast<NamedTagged_interface>(named)) {
            if (std::shared_ptr<Canonical_interface> canonical = std::dynamic_pointer_cast<Canonical_interface>(tagged)) {
                // // 引用同时具有标签和摘要，仅返回摘要。
                // // std::shared_ptr<Named_interface> newNamed = WithName(canonical->Name());
                // if (!newNamed) {
                //     throw myerror("创建新名称失败");
                // }
                // std::shared_ptr<Canonical_interface> newCanonical = WithDigest(newNamed, canonical->Digests());
                // if (!newCanonical) {
                //     throw myerror("创建新摘要失败");
                // }
                // return newCanonical;
            }
        }
        return TagNameOnly(named);
    } catch (const myerror& e) {
        // 处理myerror类型的异常
        throw; // 重新抛出异常
    }
}
