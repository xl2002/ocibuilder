#if !defined(TYPES_NETWORK_H)
#define TYPES_NETWORK_H
#include <string>
#include <vector>
#include <map>
#include <boost/asio/ip/network_v4.hpp>
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
    boost::asio::ip::address_v4 ipAddress;
    unsigned short prefixLength;

    IPNet(boost::asio::ip::address_v4 ipAddress, unsigned short prefixLength)
        : ipAddress(ipAddress), prefixLength(prefixLength) {}
};
#endif // TYPES_NETWORK_H)
