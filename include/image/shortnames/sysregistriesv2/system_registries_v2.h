#if !defined(IMAGE_SHORTNAMES_SYSREgistriesV2_SYSTEM_REGISTRIES_V2_H)
#define IMAGE_SHORTNAMES_SYSREgistriesV2_SYSTEM_REGISTRIES_V2_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <map>
#include "image/types/reference/reference.h"
#include "image/types/types.h"
#include "image/digest/digest.h"
#include "image/types/define/types.h"
#include <string>
#include "image/shortnames/sysregistriesv2/shortnames.h"
// #include <boost/property_tree/ptree.hpp>
// #include <boost/property_tree/ini_parser.hpp>

extern std::string builtinRegistriesConfPath;
extern std::string userRegistriesFile;
extern std::string userRegistriesDir;
// configWrapper 用于存储 ConfigPath 和 ConfigDirPath 的路径
// 并作为内部缓存的键。
class configWrapper {
    public:
    // 指向 registries.conf 文件的路径
    std::string configPath;

    // 指向系统范围的 registries.conf.d 目录的路径，如果未使用则为 ""
    std::string configDirPath;

    // 指向用户指定的 registries.conf.d 目录的路径，如果未使用则为 ""
    std::string userConfigDirPath;

    // 构造函数，用于初始化路径
    configWrapper()=default;
    configWrapper(const std::string& configPath, const std::string& configDirPath, const std::string& userConfigDirPath)
        : configPath(configPath), configDirPath(configDirPath), userConfigDirPath(userConfigDirPath) {}
};
// Endpoint 描述了注册表的远程位置。
class Endpoint {
    public:
    // 端点的远程位置。如果 Prefix 包含
    // 以 "*.example.com" 格式的通配符用于子域匹配，则可以为空。
    // 请参阅 FindRegistry / PullSourcesFromReference，而不是直接访问/解释 `Location`。
    std::string Location; // 位置

    // 如果为 true，将跳过证书验证，并允许 HTTP（非 TLS）
    // 连接。
    bool Insecure=false; // 不安全标志

    // PullFromMirror 用于对通过镜像拉取图像添加限制。
    // 设置为 "all"、"digest-only" 或 "tag-only"。
    // 如果为 "digest-only"，则仅使用镜像进行摘要拉取。通过
    // 标签拉取图像可能会根据从哪个端点拉取而产生不同的图像。
    // 将镜像限制为仅按摘要拉取可避免该问题。
    // 如果为 "tag-only"，则仅使用镜像进行标签拉取。对于更新频率更高且
    // 不太可能因标签移动而不同步的昂贵镜像，通常不应无故
    // 用于摘要引用。
    // 默认值为 "all"（或留空），镜像将用于摘要拉取和标签拉取，除非为主注册表设置了仅按摘要拉取镜像。
    // 此设置只能在注册表的 Mirror 字段中设置，而不能在注册表的主 Endpoint 中设置。
    // 当主注册表未配置仅按摘要拉取镜像时，允许该每个镜像设置。
    std::string PullFromMirror; // 从镜像拉取的设置
};
// Registry 表示一个注册表。
class Registry:public Endpoint {
    public:
    // Prefix 用于匹配图像，并将一个命名空间转换为另一个。
    // 如果 `Prefix="example.com/bar"`，`location="example.com/foo/bar"`，并且
    // 我们从 "example.com/bar/myimage:latest" 拉取，则图像将
    // 从 "example.com/foo/bar/myimage:latest" 拉取。
    // 如果未指定 Prefix，默认使用指定的位置。
    // Prefix 也可以采用 "*.example.com" 格式以匹配子域。
    // 通配符只能在开头，并且不应包含命名空间或特殊字符：“/”、“@”或“:”。
    std::string Prefix; // 前缀

    // 注册表也是一个端点
    // Endpoint endpoint; // 端点

    // 注册表的镜像
    std::vector<Endpoint> Mirrors; // 镜像

    // 如果为 true，将阻止从注册表拉取。
    bool Blocked=false; // 阻止标志

    // 如果为 true，镜像将仅用于摘要拉取。通过标签拉取图像可能会
    // 根据从哪个端点拉取而产生不同的图像。
    // 将镜像限制为仅按摘要拉取可避免该问题。
    bool MirrorByDigestOnly=false; // 仅按摘要拉取标志

    // 构造函数
    Registry()=default;
    // Registry() : Blocked(false), MirrorByDigestOnly(false) {}
};
// V2RegistriesConf 是 sysregistries v2 配置格式。
class V2RegistriesConf:public shortNameAliasConf {
    public:
    std::vector<Registry> Registries; // 注册表数组

    // 用于解析不合格图像引用的主机[:端口]数组（不是前缀！）
    std::map<std::string,std::string> UnqualifiedSearchRegistries; // 不合格搜索注册表数组

    // 用于身份验证的全局凭据帮助程序数组
    // （例如，["pass", "secretservice"]）。按照指定顺序查询帮助程序。
    // 注意，“containers-auth.json”是保留值，用于查询身份验证文件。
    // 如果为空，CredentialHelpers 默认为 ["containers-auth.json"]。
    std::vector<std::string> CredentialHelpers; // 凭据帮助程序数组

    // ShortNameMode 定义了短名称解析的处理方式。
    std::string ShortNameMode; // 短名称模式

    // shortNameAliasConf shortNameAliasConfig; // 短名称别名配置

    
};

class parsedConfig{
    public:
    std::shared_ptr<V2RegistriesConf> partialV2=std::make_shared<V2RegistriesConf>();
    std::string unqualifiedSearchRegistriesOrigin;
    std::shared_ptr<ShortNameMode> shortNameMode=std::make_shared<ShortNameMode>();
    std::shared_ptr<shortNameAliasCache> aliasCache=std::make_shared<shortNameAliasCache>();
};
std::shared_ptr<parsedConfig> loadConfigFile(std::string path,bool forceV2);
std::shared_ptr<parsedConfig> tryUpdatingCache(std::shared_ptr<SystemContext> ctx, std::shared_ptr<configWrapper> wrapper);
std::shared_ptr<parsedConfig> getConfig(std::shared_ptr<SystemContext> ctx);
std::shared_ptr<configWrapper> newConfigWrapperWithHomeDir(std::shared_ptr<SystemContext> ctx,std::string homeDir);
std::shared_ptr<configWrapper> newConfigWrapper(std::shared_ptr<SystemContext> ctx);
std::tuple<std::map<std::string, std::string>, std::string> UnqualifiedSearchRegistriesWithOrigin(std::shared_ptr<SystemContext> ctx);
std::map<std::string, std::string> UnqualifiedSearchRegistries(std::shared_ptr<SystemContext> ctx);
#endif // SYSREgistriesV2_SYSTEM_REGISTRIES_V2_H)
