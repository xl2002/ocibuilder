#if !defined(NETWORK_NETWORK_H)
#define NETWORK_NETWORK_H
#include <string>
#include <vector>
#include <mutex>
#include <tuple>
#include <map>
#include <boost/thread/once.hpp>
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/asio/ip/network_v4.hpp>
#include "image/types/define/types.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/filesystem.hpp>
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
namespace fs = boost::filesystem;
using tcp = asio::ip::tcp;

// #include <boost/asio/ip/network_v6.hpp>
// #include <boost/asio.hpp>  // 使用 Boost 库中的 asio 进行网络处理
class ContainerNetwork{
    public:
    std::string name;
};
struct Network {
    // 网络的名称。
    std::string Name;

    // 网络的 ID。
    std::string ID;

    // 该网络的驱动，例如 bridge, macvlan...
    std::string Driver;

    // NetworkInterface 是主机上的网络接口名称。
    std::string NetworkInterface;

    // Created 包含该网络创建时的时间戳。
    std::string Created;

    // // 要用于此网络的子网。
    // std::vector<Subnet> Subnets;

    // // 要用于此网络的路由。
    // std::vector<Route> Routes;

    // IPv6Enabled 如果设置为 true，则应为此网络创建一个 ipv6 子网。
    bool IPv6Enabled=false;

    // Internal 指示网络是否不应具有通向公共或其他网络的外部路由。
    bool Internal=false;

    // DNSEnabled 指示此网络上容器的名称解析是否激活。仅在使用 bridge 驱动时支持。
    bool DNSEnabled=false;

    // NetworkDNSServers 是 podman 的 DNS 解析器在网络级别的自定义 DNS 服务器列表，连接到该网络的所有容器都将考虑在网络级别配置的解析器。
    std::vector<std::string> NetworkDNSServers;

    // Labels 是应用于网络的一组键值标签。
    std::map<std::string, std::string> Labels;

    // Options 是应用于网络的一组键值选项。
    std::map<std::string, std::string> Options;

    // IPAMOptions 包含用于 IP 分配的选项。
    std::map<std::string, std::string> IPAMOptions;
};

struct IPNet{
    asio::ip::address_v4 ipAddress;
    unsigned short prefixLength=0;
    IPNet() = default;
    IPNet(asio::ip::address_v4 ipAddress, unsigned short prefixLength)
        : ipAddress(ipAddress), prefixLength(prefixLength) {}
};
// struct DockerAuthConfig{
//     std::string Username;
//     std::string Password;
//     std::string IdentityToken;
//     // std::string serveraddress;
// };
struct authScope{
    std::string resourceType;  
    std::string remoteName;
    std::string actions;
    std::string cookie;
    std::string harborToken;
    std::string bearerToken;
    authScope() = default;
};
extern authScope loginAuth;
struct Userinfo{
    std::string username;
    std::string password;
    bool passwordSet=false;
    Userinfo() = default;
};
extern Userinfo userinfo;
struct URL{
    std::string scheme;        // 协议方案
    std::string opaque;        // 编码的不透明数据
    std::shared_ptr<Userinfo> user; // 用户名和密码信息
    std::string host;          // 主机名或主机
    std::string port;          // 端口
    std::string projectName;   //仓库中项目名
    std::string imageName;     //镜像名
    std::string version;      //镜像版本号
    bool localPullFlag=false;     //是否从私有镜像库pull
    std::string localPullPath; //私有镜像库的地址
    std::string path;          // 路径（相对路径可能省略前导斜杠）
    std::string rawPath;       // 编码的路径提示（参见 EscapedPath 方法）
    bool omitHost=false;             // 不输出空主机（authority）
    bool forceQuery=false;           // 即使 RawQuery 为空也附加一个问号（'?'）
    std::string rawQuery;      // 编码的查询值，不包括问号（'?'）
    std::string fragment;      // 引用的片段，不包括井号（'#'）
    std::string rawFragment;   // 编码的片段提示（参见 EscapedFragment 方法）
    URL() = default;
};
struct challenge{
    std::string Scheme;
    std::map<std::string, std::string> Parameters;
    challenge() = default;
};

struct DockerAuthConfig;
struct SystemContext;
struct dockerClient{
    // 成员变量

    // 以下成员由构造函数设置，之后不再更改。
    std::shared_ptr<SystemContext> sys=std::make_shared<SystemContext>(); // 系统上下文
    std::string registry;               // 注册表地址
    std::string userAgent;              // 用户代理

    // tlsClientConfig 由构造函数设置，可以被 detectProperties() 使用和更新。
    // 调用者可以在此期间修改 tlsClientConfig->insecureSkipVerify。
    // std::shared_ptr<TlsConfig> tlsClientConfig;
    std::string tlsClientConfig;
    // 以下成员未由构造函数设置，调用者需要在必要时手动设置。
    std::shared_ptr<DockerAuthConfig> auth=std::make_shared<DockerAuthConfig>();                  // 鉴权配置
    std::string registryToken;              // 注册表令牌
    std::shared_ptr<URL> signatureBase=std::make_shared<URL>();     // 签名存储基础路径
    bool useSigstoreAttachments=false;           // 是否使用 Sigstore 附件
    std::shared_ptr<authScope> scope=std::make_shared<authScope>();                        // 鉴权范围

    // 以下成员为检测到的注册表属性：
    // 它们在 detectProperties() 成功后被设置，之后不再更改。
    std::string client; // HTTP 客户端
    std::string scheme;                // 协议方案
    std::vector<challenge> challenges; // 挑战列表
    bool supportsSignatures=false;           // 是否支持签名

    // setupRequestAuth 的私有状态（键：string，值：BearerToken）。
    std::map<std::string, std::string> tokenCache;
    std::mutex tokenCacheMutex; // 保护 tokenCache 的互斥锁

    // detectProperties 的私有状态：
    boost::once_flag detectPropertiesOnce; // 确保 detectProperties() 最多执行一次。
    boost::optional<std::string> detectPropertiesError; // 缓存初始错误。

    // logResponseWarnings 的私有状态
    std::mutex reportedWarningsLock;            // 保护 reportedWarnings 的互斥锁
    // std::set<std::string> reportedWarnings;     // 已报告的警告集合

    dockerClient()=default;
    std::shared_ptr<URL>resolveRequestURL(std::string path);
    bool setupRequestAuth(beast::http::request<beast::http::string_body> req,std::shared_ptr<authScope> extraScope);
    std::tuple<beast::http::response<beast::http::string_body>,asio::streambuf> Do(asio::io_context& ioc,std::string hosttype,beast::http::request<beast::http::string_body> req);
};
beast::http::request<beast::http::string_body> NewRequest(std::string method, std::string path, std::map<std::string, std::string> headers,std::string body);

std::string extractToken(const std::string &response_body);

std::string getToken(const std::string &host, const std::string &port, const ::string &projectName, const ::string &imagetName);

bool ifSupportV2(const std::string &host, const std::string &port);

bool ifBlobExists(const std::string &host, const std::string &port, const std::string &imageName, const std::string &shaId, const std::string &projectName);

std::pair<std::string, std::string> initUpload(const std::string &host, const std::string &port, const std::string &imageName, const std::string &projectName);

std::string uploadBlobChunk(const std::string &host, const std::string &port, const std::string &uid, const std::string &state, const std::string &file_path, std::size_t start, std::size_t end, std::size_t total_size, const std::string &imageName, const std::string &projectName);


void uploadManifest(const std::string &host, const std::string &port, const std::string &file_path, std::size_t start, std::size_t end, const std::string &imageName, const std::string version, const std::string &ManifestType, const std::string &projectName);

void finalizeUpload(const std::string &host, const std::string &port, const std::string &uid, const std::string &shaId, const std::string &state, const std::string &imageName, const std::string &projectName);

bool isCorrect(std::string sha256, std::string filepath);

void login(const std::string &user, const std::string &passwd, const std::string &host, const std::string &port);

void pullBlob(const std::string &host, const std::string &port, const ::string &projectName, const ::string &imageName, const std::string digest);

bool pullConfig(const std::string &host, const std::string &port, const ::string &projectName, const ::string &imageName, const std::string digest, const std::string &os, const std::string &arch);

std::tuple<std::string,size_t> pullManifestAndBlob(const std::string &host, const std::string &port, const ::string &projectName, const ::string &imageName, const std::string version,const std::string& os,const std::string& arch);

void saveLoginInfo(const std::string &username, const std::string &password);

void loadLoginInfo();

std::vector<std::string> getTagList(const std::string &host, const std::string &port, const ::string &projectName, const ::string &imagetName);

void getCookieFromAuthFile();

#endif // TYPES_NETWORK_H)

