#include <iostream>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

void resolve_dns(const std::string& host) {
    try {
        // 创建 Boost Asio io_service 对象
        boost::asio::io_service io_service;

        // 创建 DNS 解析器，解析给定的主机名
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "");
        
        // 获取所有匹配的 IP 地址
        boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);
        
        std::cout << "IP addresses for " << host << ":" << std::endl;

        // 使用传统迭代器方式遍历解析结果
        for (auto it = endpoints; it != boost::asio::ip::tcp::resolver::iterator(); ++it) {
            std::cout << it->endpoint().address().to_string() << std::endl;
        }
    } catch (const boost::system::system_error& e) {
        std::cerr << "Error resolving DNS: " << e.what() << std::endl;
    }
}

int main() {
    // Docker 镜像仓库的主机名
    std::string host = "swr.cn-north-4.myhuaweicloud.com";

    // 解析并输出 IP 地址
    resolve_dns(host);

    return 0;
}
