#include "image/types/reference/regexp.h"

// 常量定义

// alphaNumeric 定义了字母数字原子，通常是名称的组成部分。
// 只允许小写字母和数字。
std::string alphaNumeric;

// separator 定义了允许嵌入名称组件中的分隔符。
// 允许一个点、一个或两个下划线和多个破折号。
// 重复的破折号和下划线被有意地区别对待。
// 为了支持有效的主机名作为名称组件，添加了支持重复破折号。
// 此外，双下划线现在被允许作为分隔符，以放宽对以前支持的名称的限制。
std::string separator ;

// repository 名称以域名组件开头，并可选地跟随一个端口。
std::string domainComponent ;

// TagRegexp 的字符串对应。
std::string tag ;

// DigestRegexp 的字符串对应。
std::string digestPat ;

// IdentifierRegexp 的字符串对应。
std::string identifier ;

// ShortIdentifierRegexp 的字符串对应。
std::string shortIdentifier ;
std::string nameComponent,
    domain,referencePat,
    TagRegexpStr,anchoredIdentifier,
    DigestRegexpStr,
    IdentifierRegexpStr,
    ShortIdentifierRegexpStr;
std::string anchoredTag;
std::string anchoredDigest,namePat,anchoredName;

std::shared_ptr<std::regex> DomainRegexp,
    TagRegexp,NameRegexp,
    digestRegexp,ReferenceRegexp,
    IdentifierRegexp,
    ShortIdentifierRegexp;

std::shared_ptr<Regexp> anchoredTagRegexp;
std::shared_ptr<Regexp> anchoredDigestRegexp;
std::shared_ptr<Regexp> anchoredNameRegexp,anchoredIdentifierRegexp;


/**
 * @brief 初始化所有正则表达式模式
 * 
 * 初始化用于解析容器镜像引用的各种正则表达式模式，
 * 包括域名、标签、摘要、标识符等组件。
 * 该函数应在程序启动时调用一次。
 */
void init_regexp() {

    alphaNumeric="[a-z0-9]+";
    separator="(?:[._]|__|[-]*)";
    domainComponent="(?:[a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9])";
    tag="[\\w][\\w.-]{0,127}";
    digestPat="[A-Za-z][A-Za-z0-9]*(?:[-_+.][A-Za-z][A-Za-z0-9]*)*[:][[:xdigit:]]{32,}";
    identifier="([a-f0-9]{64})";
    shortIdentifier="([a-f0-9]{6,64})";
    nameComponent=expression({alphaNumeric,optional({repeated({separator,alphaNumeric})})});
    domain=expression({domainComponent,
                        optional({repeated({literal("."),domainComponent})}),
                        optional({literal(":"),"[0-9]+"})});
    DomainRegexp=std::make_shared<std::regex>(domain);
    TagRegexp=std::make_shared<std::regex>(tag);
    anchoredTag=anchored({tag});
    anchoredTagRegexp=Delayed(anchoredTag);
    digestRegexp=std::make_shared<std::regex>(digestPat);
    anchoredDigest=anchored({digestPat});
    anchoredDigestRegexp=Delayed(anchoredDigest);
    namePat=expression({optional({domain,literal("/")}),
                            nameComponent,
                            optional({repeated({literal("/"),nameComponent})})});
    NameRegexp=std::make_shared<std::regex>(namePat);

    anchoredName=anchored({optional({capture({domain}),literal("/")}),
                                capture({
                                    nameComponent,
                                    optional({repeated({literal("/"),nameComponent})})
                                })
                            });
    
    anchoredNameRegexp=Delayed(anchoredName);
    referencePat=anchored({capture({namePat}),
                                optional({literal(":"),capture({tag})}),
                                optional({literal("@"),capture({digestPat})})});
    ReferenceRegexp=std::make_shared<std::regex>(referencePat);
    IdentifierRegexp=std::make_shared<std::regex>(identifier);
    ShortIdentifierRegexp=std::make_shared<std::regex>(shortIdentifier);
    anchoredIdentifier=anchored({identifier});
    anchoredIdentifierRegexp=Delayed(anchoredIdentifier);
}

/**
 * @brief 转义字符串中的正则表达式保留字符
 * 
 * @param s 要转义的原始字符串
 * @return std::string 转义后的字符串
 */
std::string literal(const std::string& s) {
    std::string escaped;
    for (char c : s) {
        // 转义正则表达式中的保留字符
        if (std::string(".*+?^${}()|[]\\/").find(c) != std::string::npos) {
            escaped += '\\';  // 添加转义符
        }
        escaped += c;
    }
    return escaped;
}
// 定义一个完整的表达式，其中每个正则表达式必须跟随前一个。
/**
 * @brief 构建正则表达式字符串
 * 
 * @param res 要连接的字符串列表
 * @return std::string 连接后的正则表达式字符串
 */
std::string expression(std::initializer_list<std::string> res) {
    std::ostringstream oss;
    for (const auto& str : res) {
        oss << str;
    }
    return oss.str();
}
/**
 * @brief 构建可选的正则表达式部分
 * 
 * @param res 要设置为可选的字符串列表
 * @return std::string 添加了?量词的正则表达式
 */
std::string optional(std::initializer_list<std::string> res) {
    return group({expression(res)})+'?';
}
/**
 * @brief 构建重复的正则表达式部分
 * 
 * @param res 要设置为重复的字符串列表
 * @return std::string 添加了+量词的正则表达式
 */
std::string repeated(std::initializer_list<std::string> res) {
    auto exp=expression(res);
    return group({exp})+'+';
}
/**
 * @brief 构建非捕获组
 * 
 * @param res 要分组的字符串列表
 * @return std::string 非捕获组正则表达式
 */
std::string group(std::initializer_list<std::string> res) {
    return "(?:" + expression(res) + ')';
}
/**
 * @brief 构建捕获组
 * 
 * @param res 要捕获的字符串列表
 * @return std::string 捕获组正则表达式
 */
std::string capture(std::initializer_list<std::string> res) {
    return '(' + expression(res) + ')';
}
/**
 * @brief 构建锚定正则表达式
 * 
 * @param res 要锚定的字符串列表
 * @return std::string 添加了^和$锚点的正则表达式
 */
std::string anchored(std::initializer_list<std::string> res) {
    return '^' + expression(res) + '$';
}
