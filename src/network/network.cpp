#include "network/network.h"
#include "image/digest/digest.h"
#include "cmd/login/login.h"
#include "utils/common/go/string.h"
#include "zlib/zlib.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
/**
 * @brief 创建新的请求
 * 
 * @param method 请求的方式（HEAD，GET，POST等）
 * @param path 请求的网络路径或者target
 * @param headers 网络头的信息
 * @param body 所要传输的数据
 * @return beast::http::request<beast::http::string_body> 
 */

namespace http = boost::beast::http;
namespace ssl = boost::asio::ssl;
using boost::property_tree::ptree;

std::string base64_encode(const std::string &in)
{
    std::string out;
    out.resize(beast::detail::base64::encoded_size(in.size()));
    beast::detail::base64::encode(&out[0], in.data(), in.size());
    return out;
}

beast::http::request<beast::http::string_body> NewRequest(std::string method, std::string path, std::map<std::string, std::string> headers,std::string body){
    
    return beast::http::request<beast::http::string_body>();
}

// 用正则表达式检查是否是IP地址（IPv4）
bool isIPAddress(const std::string& host) {
    std::regex ipv4Pattern(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    return std::regex_match(host, ipv4Pattern);
}

// DNS解析函数：将URL解析为IP地址
std::string resolve_dns(const std::string& host) {
    try {
        // 创建 Boost Asio io_service 对象
        boost::asio::io_service io_service;

        // 创建 DNS 解析器，解析给定的主机名
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "");
        
        // 获取所有匹配的 IP 地址
        boost::asio::ip::tcp::resolver::iterator endpoints = resolver.resolve(query);
        
        // 返回解析到的第一个IP地址
        return endpoints->endpoint().address().to_string();
    } catch (const boost::system::system_error& e) {
        std::cerr << "Error resolving DNS: " << e.what() << std::endl;
        return "";  // 如果解析失败，返回空字符串
    }
}

/**
 * @brief 对push和pull命令根据路径解析请求的URL
 * 
 * @param path 
 * @return std::shared_ptr<URL> 
 */
std::shared_ptr<URL> dockerClient::resolveRequestURL(std::string path){
    auto url = std::make_shared<URL>();

    //如果以oci:开头 说明是从指定目录拉取
    if(path.rfind("oci:",0)==0){
        std::string imageName;
        std::string version = "latest";
        std::string pullPath;

        std::string remaining = path.substr(4);

        // 从后向前找到最后两个冒号的位置
        size_t secondColon = remaining.find_last_of(':');
        if (secondColon == std::string::npos) {
            std::cerr << "Invalid path format: No ':' found" << std::endl;
            return url;
        }

        size_t firstColon = remaining.find_last_of(':', secondColon - 1);

        pullPath = remaining.substr(0,firstColon);

        if(firstColon == secondColon){
            std::cerr << "Invalid path format"<< std::endl;
            return url;
        }else{
            imageName = remaining.substr(firstColon + 1, secondColon - firstColon - 1);
            version = remaining.substr(secondColon+1);
        }

        url->imageName=imageName;
        url->version=version;
        url->localPullFlag=true;
        url->localPullPath=pullPath;

        return url;

    }else{

        std::size_t schemePos = path.find("://");  // 查找协议部分（如果有）
        std::string pathWithoutScheme = path;  // pathWithoutScheme为跳过协议部分后的path
        std::size_t colonPos, slashPos, lastSlash;
        
        std::string host;
        std::string portStr = "";  // 默认没有端口号
        std::string pathPart = "";  // 用来保存路径部分
        std::string imageName;
        std::string version = "latest";  // 默认版本为 latest
        std::string projectName;
        
        // 如果有协议部分，跳过协议部分（即 "://"）
        if (schemePos != std::string::npos) {
            pathWithoutScheme = path.substr(schemePos + 3);  // 从协议后的部分开始处理，pathWithoutScheme为跳过协议部分后的path
        }
        
        // 现在处理没有协议部分的路径，查找冒号、斜杠等位置
        colonPos = pathWithoutScheme.find(':');    // 查找冒号，可能用于分隔主机名和端口
        slashPos = pathWithoutScheme.find('/');    // 查找第一个斜杠，可能用于分隔主机名和路径
        lastSlash = pathWithoutScheme.find_last_of('/');  // 查找最后一个斜杠，可能用于分隔路径
        
        // 如果没有斜杠，说明路径就是主机名:端口号的形式
        if (slashPos == std::string::npos) {
            if (colonPos != std::string::npos) {
                // 如果有冒号，提取主机名和端口号
                host = pathWithoutScheme.substr(0, colonPos);  // 提取主机名
                portStr = pathWithoutScheme.substr(colonPos + 1);  // 提取端口号
            } else {
                // 如果没有冒号，直接提取主机名
                host = pathWithoutScheme;
            }
        } else {
            // 路径中有斜杠，说明后面还有路径部分
            if (colonPos != std::string::npos && colonPos < slashPos) {
                // 如果冒号在斜杠之前，表示有端口号
                host = pathWithoutScheme.substr(0, colonPos);  // 提取主机名
                portStr = pathWithoutScheme.substr(colonPos + 1, slashPos - colonPos - 1);  // 提取端口号
                pathPart = pathWithoutScheme.substr(slashPos);  // 提取路径部分
            } else {
                // 如果没有冒号，提取主机名
                host = pathWithoutScheme.substr(0, slashPos);  // 提取主机名
                pathPart = pathWithoutScheme.substr(slashPos);  // 提取路径部分
            }
        }
        
        // 提取项目名
        std::size_t secondSlashAfterHost = pathWithoutScheme.find('/', slashPos + 1);  // 查找第二个斜杠，区分项目和路径部分
        
        if (secondSlashAfterHost != std::string::npos) {
            projectName = pathWithoutScheme.substr(slashPos + 1, secondSlashAfterHost - slashPos - 1);  // 提取项目名
        } else {
            projectName = pathWithoutScheme.substr(slashPos + 1);  // 如果没有找到第二个斜杠，表示没有项目名，路径直接就是镜像名
        }
        
        // 提取镜像名和版本
        std::size_t colon = pathWithoutScheme.find(':', lastSlash);  // 查找最后一个斜杠后的冒号
        
        if (colon == std::string::npos) {
            // 没有 ':'，说明没有 tag，直接返回 '/' 后的部分
            imageName = pathPart.substr(lastSlash + 1);
        } else {
            // 有 ':'，返回 ':' 之前的部分
            imageName = pathPart.substr(lastSlash + 1, colon - lastSlash - 1);
            version = pathPart.substr(colon + 1);
        }
        
        // 检查是否有明确的协议
        std::shared_ptr<URL> url = std::make_shared<URL>();
        if (schemePos != std::string::npos) {
            if (hasPrefix(path, "http://")) {
                url->scheme = "http";  // 如果是以 http:// 开头，使用 http 协议
            } else if (hasPrefix(path, "https://")) {
                url->scheme = "https";  // 如果是以 https:// 开头，使用 https 协议
            }
        } else {
            // 如果没有明确的协议，判断是IP地址还是URL
            if (isIPAddress(host)) {
                url->scheme = "http";  // 如果是IP地址，使用 http 协议
            } else {
                // URL包含字母和点，进行DNS解析
                url->scheme = "https";  // 否则默认为 https 协议
            }
        }
        
        // 如果没有给定端口号，根据协议设置默认端口
        if (portStr.empty()) {
            if (url->scheme == "http") {
                portStr = "5000";  // http 协议默认端口为 5000
            } else if (url->scheme == "https") {
                portStr = "443";  // https 协议默认端口为 443
            }
        }
        
        // 对于URL格式的主机名，需要进行DNS解析
        if (!isIPAddress(host)) {
            host = resolve_dns(host);  // 使用DNS解析将主机解析为IP地址
            if (host.empty()) {
                std::cerr << "DNS resolution failed for host: " << host << std::endl;
                return url;
            }
        }
        
        url->host = host;
        url->port = portStr;
        url->imageName = imageName;
        url->version = version;
        url->projectName = projectName;
        
        return url;
    }

    
}

/**
 * @brief 对login命令根据路径解析请求的URL
 * 
 * @param path 
 * @return std::shared_ptr<URL> 
 */
std::shared_ptr<URL>dockerClient::resolveLoginURL(std::string path){
    std::size_t schemePos = path.find("://");  // 查找协议部分（如果有）
    std::string pathWithoutScheme = path;  // pathWithoutScheme为跳过协议部分后的path
    std::size_t colonPos, slashPos;
    
    std::string host;
    std::string portStr = "";  // 默认没有端口号

    // 如果有协议部分，跳过协议部分（即 "://"）
    if (schemePos != std::string::npos) {
        pathWithoutScheme = path.substr(schemePos + 3);  // 从协议后的部分开始处理
    }
    
    // 现在处理没有协议部分的路径，查找冒号、斜杠等位置
    colonPos = pathWithoutScheme.find(':');    // 查找冒号，可能用于分隔主机名和端口
    slashPos = pathWithoutScheme.find('/');    // 查找第一个斜杠，可能用于分隔主机名和路径

    // 路径是主机名:端口号的形式
    if (colonPos != std::string::npos) {
        // 如果有冒号，提取主机名和端口号
        host = pathWithoutScheme.substr(0, colonPos);  // 提取主机名
        portStr = pathWithoutScheme.substr(colonPos + 1);  // 提取端口号
    } else {
        // 如果没有冒号，直接提取主机名
        host = pathWithoutScheme;
    }

    // 检查是否有明确的协议
    std::shared_ptr<URL> url = std::make_shared<URL>();
    if (schemePos != std::string::npos) {
        if (hasPrefix(path, "http://")) {
            url->scheme = "http";  // 如果是以 http:// 开头，使用 http 协议
        } else if (hasPrefix(path, "https://")) {
            url->scheme = "https";  // 如果是以 https:// 开头，使用 https 协议
        }
    } else {
        // 如果没有明确的协议，判断是IP地址还是URL
        if (isIPAddress(host)) {
            url->scheme = "http";  // 如果是IP地址，使用 http 协议
        } else {
            url->scheme = "https";  // 否则默认为 https 协议
        }
    }

    // 如果没有给定端口号，根据协议设置默认端口
    if (portStr.empty()) {
        if (schemePos != std::string::npos && hasPrefix(path, "http://")) {
            portStr = "5000";  // http 协议默认端口为 5000
        } else {
            portStr = "443";  // https 协议默认端口为 443
        }
    }

    // 对于URL格式的主机名，需要进行DNS解析
    if (!isIPAddress(host)) {
        host = resolve_dns(host);  // 使用DNS解析将主机解析为IP地址
        if (host.empty()) {
            std::cerr << "DNS resolution failed for host: " << host << std::endl;
            return url;
        }
    }

    // 设置返回的URL对象
    url->host = host;
    url->port = portStr;

    return url;

}

/**
 * @brief 获得认证令牌，获得的令牌直接存储到dockerClient中
 * 
 * @param req 
 * @param extraScope 
 * @return true 
 * @return false 
 */
bool dockerClient::setupRequestAuth(beast::http::request<beast::http::string_body> req,std::shared_ptr<authScope> extraScope){
    return true;
}
/**
 * @brief 发送请求并获得响应
 * 
 * @param ioc 
 * @param hosttype 
 * @param req 
 * @return std::tuple<beast::http::response<beast::http::string_body>,asio::streambuf> 
 */
std::tuple<beast::http::response<beast::http::string_body>,asio::streambuf> dockerClient::Do(asio::io_context& ioc,std::string hosttype,beast::http::request<beast::http::string_body> req){
    
}


std::string extractToken(const std::string& response_body){
    // 查找 "token" 字段的位置
    std::string token_key = "\"token\":\"";
    size_t token_pos = response_body.find(token_key);
    
    if (token_pos != std::string::npos) {
        // 提取 token 字段
        size_t start = token_pos + token_key.length();
        size_t end = response_body.find("\"", start);
        
        if (end != std::string::npos) {
            std::string token = response_body.substr(start, end - start);
            return token;
        } else {
            return "";
        }
    } else {
        return "";
    }
}


std::string getToken(const std::string& host, const std::string& port,const::string& projectName,const::string& imagetName){
    // std::string target="/service/token?service=harbor-registry&scope=repository:library/busybox:pull";
    std::string target="/service/token?service=harbor-registry&scope=repository:"+projectName+"/"+imagetName+":pull";


    // IO 上下文
    asio::io_context ioc;

    // 解析器和流
    asio::ip::tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // 解析并连接到主机
    auto const results = resolver.resolve(host, port);
    stream.connect(results);

    // 构造 HTTP HEAD 请求
    beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11);
    req.set(beast::http::field::host,host);
    req.set(beast::http::field::cookie,loginAuth.cookie);
    req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> res;
    beast::http::read(stream, buffer, res);


    std::string harbor_csrf_token;
    for (auto const& field : res) {
        if (field.name_string() == "X-Harbor-Csrf-Token") {
            std::string token = field.value().to_string();
            loginAuth.harborToken=token;
            break;
        }
    }
    std::string btoken=extractToken(res.body());
    loginAuth.bearerToken=btoken;
    return btoken;

}


/**
 * @brief 验证服务器是否支持v2协议
 * 
 * @param host 
 * @param port 
 * @return 
 */
bool ifSupportV2(const std::string& host,const std::string& port){
    try {
        std::string target = "/v2/";  // API v2 路径

        // IO 服务和解析器
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 设置超时时间
        stream.expires_after(std::chrono::seconds(5));  // 超时 5 秒

        // 解析并连接
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 设置 HTTP 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11);  // 发送 HEAD 请求
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set(beast::http::field::user_agent, "Boost.Beast/248");

        // 发送请求
        beast::http::write(stream, req);
        std::cout << "HTTP request sent." << std::endl;

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);
        std::cout << "Response received." << std::endl;
        // 关闭连接
        stream.close(); 
        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
            }
        }
        // 检查响应状态码
        if (res.result() == beast::http::status::ok) {
            return true;
        } else {
            return false;
        }


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// acore的认证方式采用用户名+密码
void setAuthorization(beast::http::request<beast::http::empty_body>& req, const std::string& user, const std::string& passwd) {
    std::string credentials = user + ":" + passwd;
    std::string encoded_credentials = base64_encode(credentials);
    req.set(http::field::authorization, "Basic " + encoded_credentials);
}

// harbor的认证方式采用cookie+token
void setAuthorization(beast::http::request<beast::http::empty_body>& req, const std::string& token) {
    if (!token.empty()) {
        req.set(http::field::authorization, "Bearer " + token);
    }
}


/**
 * @brief 该数据层是否在服务器中存在
 * @param host 
 * @param port 
 * @param imageName 
 * @param shaId 
 * @param projectName 
 * @param scheme 协议类型，支持 "http" 或 "https"
 * @return 
 */
bool ifBlobExists(const std::string& host, const std::string& port, const std::string& imageName, const std::string& shaId, const std::string& projectName, const std::string& scheme) {
    try {
        // 配置参数
        const std::string target = "/v2/" + projectName + "/" + imageName + "/blobs/sha256:" + shaId;

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 解析并连接到主机
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP HEAD 请求
            http::request<http::empty_body> req(http::verb::head, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::dynamic_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 根据状态码判断 blob 是否存在
            if (res.result() == http::status::ok) {
                return true;
            }

            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);

            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }
            // 解析并连接
            auto const results = resolver.resolve(host, port);
            beast::get_lowest_layer(stream).connect(results);
            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构造 HTTP HEAD 请求
            http::request<http::empty_body> req(http::verb::head, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::dynamic_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 根据状态码判断 blob 是否存在
            if (res.result() == http::status::ok) {
                return true;
            }

            // 关闭连接
            beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
        std::cerr << "Blob Exist Error: " << e.what() << "\n";
    }
    return false;
}



/**
 * @brief 上传数据前初始化请求，以获得uid和state
 * @param host 
 * @param port 
 * @param imageName 
 * @param projectName 
 * @param scheme 协议类型，支持 "http" 或 "https"
 * @return 
 */
 std::pair<std::string, std::string> initUpload(const std::string& host, const std::string& port, const std::string& imageName, const std::string& projectName, const std::string& scheme) {
    try {
        const std::string target = "/v2/" + projectName + "/" + imageName + "/blobs/uploads/";

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 解析并连接到主机
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP POST 请求
            http::request<http::empty_body> req(http::verb::post, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            req.set(http::field::accept_encoding, "gzip");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::dynamic_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 检查响应状态
            if (res.result() != http::status::accepted) {
                throw std::runtime_error("Failed to initiate upload");
            }

            // 提取 location、uid 和 state
            std::string location = res[http::field::location].to_string();
            std::string ipAddr = host + ":" + port;
            auto pos_start = location.find(ipAddr);
            if (pos_start != std::string::npos) {
                loginAuth.location = location.substr(pos_start + ipAddr.length());
            }

            std::string uid_param;
            std::string state_param;

            // 获取 uid
            for (auto const& field : res) {
                if (field.name_string() == "Docker-Upload-Uuid") {
                    uid_param = field.value().to_string();
                }
            }

            // 获取 state
            auto pos = location.find("?_state=");
            if (pos != std::string::npos) {
                state_param = location.substr(pos + 8);
            } else {
                pos = location.find("?state_=");
                if (pos != std::string::npos) {
                    state_param = location.substr(pos + 8);
                } else {
                    throw std::runtime_error("Failed to get state");
                }
            }

            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            return {uid_param, state_param};
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);
            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }
            // 解析并连接
            auto const results = resolver.resolve(host, port);
            beast::get_lowest_layer(stream).connect(results);
            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构造 HTTP POST 请求
            http::request<http::empty_body> req(http::verb::post, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            req.set(http::field::accept_encoding, "gzip");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::dynamic_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 检查响应状态
            if (res.result() != http::status::accepted) {
                throw std::runtime_error("Failed to initiate upload");
            }

            // 提取 location、uid 和 state
            std::string location = res[http::field::location].to_string();
            std::string ipAddr = host + ":" + port;
            auto pos_start = location.find(ipAddr);
            if (pos_start != std::string::npos) {
                loginAuth.location = location.substr(pos_start + ipAddr.length());
            }

            std::string uid_param;
            std::string state_param;

            // 获取 uid
            for (auto const& field : res) {
                if (field.name_string() == "Docker-Upload-Uuid") {
                    uid_param = field.value().to_string();
                }
            }

            // 获取 state
            auto pos = location.find("?_state=");
            if (pos != std::string::npos) {
                state_param = location.substr(pos + 8);
            } else {
                pos = location.find("?state_=");
                if (pos != std::string::npos) {
                    state_param = location.substr(pos + 8);
                } else {
                    throw std::runtime_error("Failed to get state");
                }
            }

            // 关闭连接
            beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            return {uid_param, state_param};
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
        std::cerr << "Init Upload Error: " << e.what() << "\n";
        return {"", ""};
    }
}




/**
 * @brief 以层为单位上传数据
 * @param host 
 * @param port 
 * @param uid 
 * @param state 
 * @param file_path 
 * @param start 
 * @param end 
 * @param total_size 
 * @param imageName 
 * @param projectName 
 * @param scheme 协议类型，支持 "http" 或 "https"
 * @return 
 */
 std::pair<std::string, std::string> uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
    const std::string& file_path, std::size_t start, std::size_t end, std::size_t total_size, const std::string& imageName, const std::string& projectName, const std::string& scheme) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }

        std::size_t chunk_size = end - start;
        std::vector<char> data(chunk_size);

        file.seekg(start, std::ios::beg);
        file.read(data.data(), chunk_size);
        std::size_t bytes_read = file.gcount();

        if (file.gcount() != static_cast<std::streamsize>(chunk_size)) {
            throw std::runtime_error("Failed to read the requested chunk from file.");
        }

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
        // 创建 TCP 流
        beast::tcp_stream stream(ioc);

        // 解析并连接到主机
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 构造 HTTP PATCH 请求
        std::string target = loginAuth.location;
        http::request<http::buffer_body> req(http::verb::patch, target, 11);
        req.set(http::field::host, host + ":" + port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
        req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
        } else {
        std::string credentials = userinfo.username + ":" + userinfo.password;
        std::string encoded_credentials = base64_encode(credentials);
        req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(http::field::content_type, "application/octet-stream");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read;
        req.body().more = false;
        req.prepare_payload();

        // 发送请求
        http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        http::read(stream, buffer, parser);
        auto res = parser.get();

        // 检查响应状态
        if (res.result() != http::status::accepted) {
        throw std::runtime_error("Failed to upload blob chunk");
        }

        // 提取 location、new_uid 和 new_state
        std::string location = res[http::field::location].to_string();
        std::string ipAddr = host + ":" + port;
        auto pos_start = location.find(ipAddr);
        if (pos_start != std::string::npos) {
        loginAuth.location = location.substr(pos_start + ipAddr.length());
        }

        std::string new_uid, new_state;
        for (auto const& field : res) {
        if (field.name_string() == "Docker-Upload-Uuid") {
        new_uid = field.value().to_string();
        }
        }

        // 使用 acore 方式
        if (loginAuth.bearerToken.empty()) {
        auto pos = location.find("state_=");
        if (pos != std::string::npos) {
        new_state = location.substr(pos + 7);
        }
        } 
        // 使用 harbor 方式
        else {
        auto pos = location.find("_state=");
        if (pos != std::string::npos) {
        new_state = location.substr(pos + 7);
        }
        }

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

        return {new_uid, new_state};
        } else if (scheme == "https") {
        // 创建 SSL 上下文
        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
        // 创建 SSL 流
        ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        // 设置 SNI 主机名
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw boost::system::system_error(
        boost::system::error_code(
        static_cast<int>(::ERR_get_error()),
        boost::asio::error::get_ssl_category()));
        }
        // 解析并连接
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        // SSL 握手
        stream.handshake(ssl::stream_base::client);

        // 构造 HTTP PATCH 请求
        std::string target = loginAuth.location;
        http::request<http::buffer_body> req(http::verb::patch, target, 11);
        req.set(http::field::host, host + ":" + port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
        req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
        } else {
        std::string credentials = userinfo.username + ":" + userinfo.password;
        std::string encoded_credentials = base64_encode(credentials);
        req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(http::field::content_type, "application/octet-stream");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read;
        req.body().more = false;
        req.prepare_payload();

        // 发送请求
        http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        http::read(stream, buffer, parser);
        auto res = parser.get();

        // 检查响应状态
        if (res.result() != http::status::accepted) {
        throw std::runtime_error("Failed to upload blob chunk");
        }

        // 提取 location、new_uid 和 new_state
        std::string location = res[http::field::location].to_string();
        std::string ipAddr = host + ":" + port;
        auto pos_start = location.find(ipAddr);
        if (pos_start != std::string::npos) {
        loginAuth.location = location.substr(pos_start + ipAddr.length());
        }

        std::string new_uid, new_state;
        for (auto const& field : res) {
        if (field.name_string() == "Docker-Upload-Uuid") {
        new_uid = field.value().to_string();
        }
        }

        // 使用 acore 方式
        if (loginAuth.bearerToken.empty()) {
        auto pos = location.find("state_=");
        if (pos != std::string::npos) {
        new_state = location.substr(pos + 7);
        }
        } 
        // 使用 harbor 方式
        else {
        auto pos = location.find("_state=");
        if (pos != std::string::npos) {
        new_state = location.substr(pos + 7);
        }
        }

        // 关闭连接
        beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);

        return {new_uid, new_state};
        } else {
        throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
    std::cerr << "Upload Blob Chunk Error: " << e.what() << "\n";
    return {uid, state};
    }
    }

void getManifest(ptree& node, const std::string& level)
{
    try {
        for (auto& child : node) {
            // 如果当前节点是 "mediaType" 键，修改它的值
            if (child.first == "mediaType") {
                if (level == "root") {
                    child.second.put_value("application/vnd.oci.image.manifest.v1+json");  // 外层mediaType
                } else if (level == "config") {
                    child.second.put_value("application/vnd.oci.config.v1+json");  // config层的mediaType
                } else if (level == "layers") {
                    child.second.put_value("application/vnd.oci.image.layer.v1.tar+gzip");  // layers层的mediaType
                }
            }
            // 递归处理子节点，根据不同的层级传递相应的值
            if (child.first == "config") {
                getManifest(child.second, "config");
            } else if (child.first == "layers") {
                getManifest(child.second, "layers");
            } else if (child.first != "mediaType") {
                getManifest(child.second, level);
            }
        } 
    } catch(const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::string write_v1_manifest(const std::string& filePath)
{
    ptree manifest;
    boost::property_tree::read_json(filePath, manifest);
    getManifest(manifest, "root");
    std::string output_path = "oci_images/tmp/manifest.json";
    std::ofstream output_file(output_path);
    boost::property_tree::write_json(output_file, manifest, false);
    output_file.close();
    auto digest = Fromfile(output_path);
    std::string shaId = digest->Encoded();
    // 最后给文件重命名
    std::string new_path = "oci_images/tmp/" + shaId;
    boost::filesystem::rename(output_path, new_path);
    return new_path;
}

std::string write_manifest_new(const std::string& file_path)
{
    std::ifstream ifs(file_path);
    std::stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    auto manifest = unmarshal<::Manifest>(ss.str());
    manifest.MediaType = "application/vnd.oci.image.manifest.v1+json";
    manifest.Config.MediaType = "application/vnd.oci.image.config.v1+json";
    for (auto& layer : manifest.Layers) {
        layer.MediaType = "application/vnd.oci.image.layer.v1.tar+gzip";
    }
    std::string m_str = marshal<::Manifest>(manifest);
    std::ofstream ofs("oci_images/tmp/manifest.json");
    ofs << m_str;
    ofs.close();
    auto digest = Fromfile("oci_images/tmp/manifest.json");
    std::string shaId = digest->Encoded();
    std::string new_path = "oci_images/tmp/" + shaId;
    boost::filesystem::rename("oci_images/tmp/manifest.json", new_path);
    return new_path;
}

/**
 * @brief 上传 manifest
 * @param host 
 * @param port 
 * @param file_path 
 * @param start 
 * @param end 
 * @param imageName 
 * @param version 
 * @param ManifestType 
 * @param projectName 
 * @param v1 
 * @param scheme 协议类型，支持 "http" 或 "https"
 */
void uploadManifest(const std::string& host, const std::string& port, const std::string& file_path, std::size_t start, std::size_t end,
    const std::string& imageName, const std::string& version, const std::string& ManifestType, const std::string& projectName, bool v1, const std::string& scheme) {
    try {
        std::size_t chunk_size = end - start;
        std::vector<char> data(chunk_size);
        std::size_t bytes_read;

        // 如果采用 v1 版本，则需要创建一个临时的 manifest 文件
        if (v1 == true) {
            std::string new_path = write_manifest_new(file_path);
            std::cout << "read file" << std::endl;
            std::ifstream file(file_path, std::ios::binary);
            file.seekg(start, std::ios::beg);
            file.read(data.data(), chunk_size);
            bytes_read = file.gcount();
        } else {
            std::ifstream file(file_path, std::ios::binary);
            file.seekg(start, std::ios::beg);
            file.read(data.data(), chunk_size);
            bytes_read = file.gcount();
        }

            // IO 上下文
            asio::io_context ioc;

            // 解析器和流
            asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 解析并连接到主机
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP PUT 请求
            std::string target = "/v2/" + projectName + "/" + imageName + "/manifests/" + version;
            http::request<http::buffer_body> req(http::verb::put, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
        } else {
            std::string credentials = userinfo.username + ":" + userinfo.password;
            std::string encoded_credentials = base64_encode(credentials);
            req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(http::field::content_type, ManifestType);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read;
        req.body().more = false;
        req.prepare_payload();

        // 发送请求
        http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        http::read(stream, buffer, parser);
        auto res = parser.get();

        // 检查响应状态
        if (res.result() != http::status::ok && res.result() != http::status::created) {
            throw std::runtime_error("Failed to upload manifest");
        }

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else if (scheme == "https") {
        // 创建 SSL 上下文
        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
        // 创建 SSL 流
        ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        // 设置 SNI 主机名
        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            throw boost::system::system_error(
                boost::system::error_code(
                    static_cast<int>(::ERR_get_error()),
                    boost::asio::error::get_ssl_category()));
        }
        // 解析并连接
        auto const results = resolver.resolve(host, port);
        beast::get_lowest_layer(stream).connect(results);
        // SSL 握手
        stream.handshake(ssl::stream_base::client);

        // 构造 HTTP PUT 请求
        std::string target = "/v2/" + projectName + "/" + imageName + "/manifests/" + version;
        http::request<http::buffer_body> req(http::verb::put, target, 11);
        req.set(http::field::host, host + ":" + port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
        } else {
            std::string credentials = userinfo.username + ":" + userinfo.password;
            std::string encoded_credentials = base64_encode(credentials);
            req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(http::field::content_type, ManifestType);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read;
        req.body().more = false;
        req.prepare_payload();

        // 发送请求
        http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        http::response_parser<http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        http::read(stream, buffer, parser);
        auto res = parser.get();

        // 检查响应状态
        if (res.result() != http::status::ok && res.result() != http::status::created) {
            throw std::runtime_error("Failed to upload manifest");
        }

        // 关闭连接
        beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
    }
} catch (const std::exception& e) {
std::cerr << "Upload Manifest Error: " << e.what() << "\n";
}
}


/**
 * @brief 每次数据上传完之后的确认请求
 * @param host 
 * @param port 
 * @param uid 
 * @param shaId 
 * @param state 
 * @param imageName 
 * @param projectName 
 * @param scheme 协议类型，支持 "http" 或 "https"
 */
 void finalizeUpload(const std::string& host, const std::string& port, const std::string& uid, const std::string& shaId, const std::string& state, const std::string& imageName, const std::string& projectName, const std::string& scheme) {
    try {
        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 解析并连接到主机
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP PUT 请求
            std::string target = loginAuth.location + "&digest=sha256:" + shaId;
            http::request<http::empty_body> req(http::verb::put, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set(http::field::content_type, "application/octet-stream");
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            req.set(http::field::accept_encoding, "gzip");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::empty_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 检查响应状态
            if (res.result() != http::status::created) {
                throw std::runtime_error("Failed to finalize upload");
            }

            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);
            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }
            // 解析并连接
            auto const results = resolver.resolve(host, port);
            beast::get_lowest_layer(stream).connect(results);
            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构造 HTTP PUT 请求
            std::string target = loginAuth.location + "&digest=sha256:" + shaId;
            http::request<http::empty_body> req(http::verb::put, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set(http::field::content_type, "application/octet-stream");
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set(http::field::connection, "close");
            req.set(http::field::accept_encoding, "gzip");
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, userinfo.username, userinfo.password);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response_parser<http::empty_body> parser;
            parser.body_limit(std::numeric_limits<std::uint64_t>::max());
            parser.skip(true);
            http::read(stream, buffer, parser);
            auto res = parser.get();

            // 检查响应状态
            if (res.result() != http::status::created) {
                throw std::runtime_error("Failed to finalize upload");
            }

            // 关闭连接
            beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
        std::cerr << "Finalize Upload Error: " << e.what() << "\n";
    }
}

bool isCorrect(std::string sha256,std::string filepath){
    auto digest = Fromfile(filepath);
    return digest->Encoded() == sha256;
}


std::string gzDecompressToString(const std::string& compressed) {
    z_stream strm;
    strm.zalloc = NULL;
    strm.zfree = NULL;
    strm.opaque = NULL;

    int ret;
    ret = inflateInit2(&strm, MAX_WBITS + 16); // MAX_WBITS+16 is for handling GZIP headers
    if (ret != Z_OK) {
        throw std::runtime_error("inflateInit2 failed");
    }

    strm.avail_in = compressed.size();
    strm.next_in = (Bytef *)compressed.data();

    // Prepare an output string for decompressed data
    std::string decompressed;
    decompressed.resize(compressed.size() * 2); // Pre-allocate enough space, it will be resized later

    strm.avail_out = decompressed.size();
    strm.next_out = (Bytef *)decompressed.data();

    ret = inflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        inflateEnd(&strm);
        throw std::runtime_error("Decompression failed");
    }

    // Resize the output string to the actual decompressed size
    decompressed.resize(strm.total_out);

    inflateEnd(&strm);
    return decompressed;
}

std::string loginGetToken(std::string host, std::string port, std::string user, std::string passwd, std::string projectName, std::string imageName)
{
    asio::io_context ioc;
    int version = 11;
    // 第二次发送请求
    beast::tcp_stream stream2(ioc);
    asio::ip::tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);
    stream2.connect(results);

    std::string target2;
    if (!projectName.empty())
        target2 = "/service/token?account=admin&scope=repository%3A" + projectName + "/" + imageName + ":pull%2Cpush&service=harbor-registry";
    else
        target2 = "/service/token?account=admin&service=harbor-registry";
    http::request<http::empty_body> req2{http::verb::get, target2, version};
    // 设置用户的登录信息
    setAuthorization(req2, user, passwd);
    req2.set(http::field::host, host+":"+port);
    req2.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req2.set("Accept-Encoding", "gzip");
    req2.set(http::field::connection, "close");
    http::write(stream2, req2);

    // 第二次接收响应
    beast::flat_buffer buffer2;
    http::response<http::string_body> res2;
    http::read(stream2, buffer2, res2);

    // 需要从接收到的响应中读取token
    if (res2.result() == http::status::ok) {
        std::string btoken;
        bool is_gzip = false;
        for (auto const& field : res2) {
            // 如果接收到的json是gzip压缩的，需要解压
            if (field.name_string() == "Content-Encoding" && field.value() == "gzip") {
                btoken = extractToken(gzDecompressToString(res2.body()));
                is_gzip = true;
            }
        }
        std::string sid;
        // 获取cookie的长度
        for (auto const& field : res2) {
            if (field.name_string() == "Set-Cookie") {

                std::string cookie = field.value().to_string();
                std::size_t sidpos=cookie.find("sid=");
                if(sidpos!=std::string::npos){
                    sidpos+=4;
                    std::size_t end_sid = cookie.find(";", sidpos);
                    if (end_sid == std::string::npos) {
                        end_sid = cookie.length(); // 如果没有分号，则取到字符串结尾
                    }
                    sid=cookie.substr(sidpos,end_sid-sidpos);
                    loginAuth.cookie.append("sid=" + sid + "; ");
                    // loginAuth.cookie="_gorilla_csrf="+csrf_token+"; sid="+sid+";";
                }
            }
        }
        if (!is_gzip) {
            btoken = extractToken(res2.body());
        }
        return btoken;
    }
    beast::get_lowest_layer(stream2).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    stream2.socket().shutdown(tcp::socket::shutdown_both);
    return "";
}

// login最后需要获取一个token，如果有这个token，则将其嵌入到authroization中，否则authorization是用户名和密码
std::string login_and_getToken(const std::string& user, const std::string& passwd, const std::string& host, const std::string& port, 
    const std::string& projectName, const std::string& imageName,const std::string& scheme) {
        try {
            // 创建 I/O 上下文
            asio::io_context ioc;
    
            // 解析主机名和端口
            std::string target = "/v2/";  // 根据需要修改路径
            int version = 11;  // HTTP 版本，1.1
    
            // 创建 TCP 解析器
            asio::ip::tcp::resolver resolver(ioc);
            auto const results = resolver.resolve(host, port);
    
            // 根据协议选择不同的流类型
            if (scheme == "http") {
                // 创建 TCP 流
                beast::tcp_stream stream(ioc);
                stream.connect(results);
    
                // 构建 HTTP 请求
                http::request<http::empty_body> req{http::verb::get, target, version};
                // 设置其他头部信息
                req.set(http::field::host, host + ":" + port);
                req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                req.set("Docker-Distribution-Api-Version", "registry/2.0");
                req.set("Accept-Encoding", "gzip");
                req.set(http::field::connection, "close");
    
                // 第一次发送请求
                http::write(stream, req);
    
                // 第一次接收响应
                beast::flat_buffer buffer;
                http::response<http::dynamic_body> res;
                http::read(stream, buffer, res);
    
                if (res.result() == http::status::unauthorized) {
                    bool receive_cookie = false;
                    std::string sid;
                    // 获取cookie的长度
                    for (auto const& field : res) {
                        if (field.name_string() == "Set-Cookie") {
                            receive_cookie = true;
    
                            std::string cookie = field.value().to_string();
                            std::size_t sidpos = cookie.find("sid=");
                            if (sidpos != std::string::npos) {
                                sidpos += 4;
                                std::size_t end_sid = cookie.find(";", sidpos);
                                if (end_sid == std::string::npos) {
                                    end_sid = cookie.length(); // 如果没有分号，则取到字符串结尾
                                }
                                sid = cookie.substr(sidpos, end_sid - sidpos);
                                loginAuth.cookie.append("sid=" + sid + "; ");
                            }
                        }
                    }
                    // 关闭连接
                    stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
                    // 如果接受到cookie，说明是harbor方式，需要继续发送请求
                    if (receive_cookie) {
                        // 第二次发送请求
                        std::string btoken = loginGetToken(host, port, user, passwd, projectName, imageName);
                        return btoken;
                    }
                }
            } else if (scheme == "https") {
                // 创建 SSL 上下文
                boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
                // 创建 SSL 流
                ssl::stream<beast::tcp_stream> stream(ioc, ctx);
                // 设置 SNI 主机名
                if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                    throw boost::system::system_error(
                        boost::system::error_code(
                            static_cast<int>(::ERR_get_error()),
                            boost::asio::error::get_ssl_category()));
                }
                // 连接
                beast::get_lowest_layer(stream).connect(results);
                // SSL 握手
                stream.handshake(ssl::stream_base::client);
    
                // 构建 HTTP 请求
                http::request<http::empty_body> req{http::verb::get, target, version};
                // 设置其他头部信息
                req.set(http::field::host, host + ":" + port);
                req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
                req.set("Docker-Distribution-Api-Version", "registry/2.0");
                req.set("Accept-Encoding", "gzip");
                req.set(http::field::connection, "close");
    
                // 第一次发送请求
                http::write(stream, req);
    
                // 第一次接收响应
                beast::flat_buffer buffer;
                http::response<http::dynamic_body> res;
                http::read(stream, buffer, res);
    
                if (res.result() == http::status::unauthorized) {
                    bool receive_cookie = false;
                    std::string sid;
                    // 获取cookie的长度
                    for (auto const& field : res) {
                        if (field.name_string() == "Set-Cookie") {
                            receive_cookie = true;
    
                            std::string cookie = field.value().to_string();
                            std::size_t sidpos = cookie.find("sid=");
                            if (sidpos != std::string::npos) {
                                sidpos += 4;
                                std::size_t end_sid = cookie.find(";", sidpos);
                                if (end_sid == std::string::npos) {
                                    end_sid = cookie.length(); // 如果没有分号，则取到字符串结尾
                                }
                                sid = cookie.substr(sidpos, end_sid - sidpos);
                                loginAuth.cookie.append("sid=" + sid + "; ");
                            }
                        }
                    }
                    // 关闭连接
                    beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
                    // 如果接受到cookie，说明是harbor方式，需要继续发送请求
                    if (receive_cookie) {
                        // 第二次发送请求
                        std::string btoken = loginGetToken(host, port, user, passwd, projectName, imageName);
                        return btoken;
                    }
                }
            } else {
                throw std::invalid_argument("Unsupported scheme: " + scheme);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        return "";
    }

bool login(const std::string& host, const std::string& port, const std::string& user, const std::string& passwd, const std::string& scheme) {
    try {
        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
        std::string target = "/v2/";  // 根据需要修改路径
        int version = 11;  // HTTP 版本，1.1

        // 创建 TCP 解析器
        asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);
            stream.connect(results);

            // 构建 HTTP 请求
            http::request<http::empty_body> req{http::verb::get, target, version};
            // 设置其他头部信息
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set("Accept-Encoding", "gzip");
            req.set(http::field::connection, "close");

            // 设置 Authorization
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, user, passwd);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::dynamic_body> res;
            http::read(stream, buffer, res);

            // 检查响应状态
            if (res.result() == http::status::ok) {
                std::cout << "Login success!!" << std::endl;
                return true;
            }
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);
            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);
            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }
            // 连接
            beast::get_lowest_layer(stream).connect(results);
            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构建 HTTP 请求
            http::request<http::empty_body> req{http::verb::get, target, version};
            // 设置其他头部信息
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-Api-Version", "registry/2.0");
            req.set("Accept-Encoding", "gzip");
            req.set(http::field::connection, "close");

            // 设置 Authorization
            if (!loginAuth.bearerToken.empty()) {
                setAuthorization(req, loginAuth.bearerToken);
            } else {
                setAuthorization(req, user, passwd);
            }

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::dynamic_body> res;
            http::read(stream, buffer, res);

            // 检查响应状态
            if (res.result() == http::status::ok) {
                std::cout << "Login success!!" << std::endl;
                return true;
            }
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return false;
}

void pullBlob(const std::string& host, const std::string& port,const::string& projectName,const::string& imageName,const std::string digest){
    // std::string target = "/v2/library/busyboximage5/blobs/sha256:f28efabc598d38f0b7cea1641bd20b097b8c5aaf090035d7167370608d86fb67"; // API v2 路径
    std::string target= "/v2/"+projectName+"/"+imageName+"/blobs/"+digest;
    std::string output_folder = "oci_images/oci_registry/blobs/sha256"; // 保存的文件夹
    std::string shaId=digest.substr(7);
    std::string output_file = output_folder + "/" + shaId; // 完整的文件路径
    std::string output_tmp = "oci_images/tmp/"+shaId; //临时存储 

    try {
        // 确保文件夹存在
        if (!fs::exists(output_folder)) {
            fs::create_directory(output_folder);
            std::cout << "Created directory: " << output_folder << std::endl;
        }

        //如果库中存在这一层数据则直接退出
        if(fs::exists(output_file)){
            return;
        }

        // IO 服务和解析器
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 设置超时时间
        stream.expires_after(std::chrono::seconds(5)); // 超时 5 秒

        // 解析并连接
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 设置 HTTP 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11); // 使用 GET 请求
        req.set(beast::http::field::host, host+":"+port);
        req.set(beast::http::field::connection, "close");
        req.set("Accept-Encoding", "gzip");
        req.set("Docker-Distribution-API-Version", "registry/2.0");
        if(loginAuth.bearerToken.empty()){
            setAuthorization(req, userinfo.username, userinfo.password);
        }
        else{
            req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
            // req.set(beast::http::field::cookie,loginAuth.cookie);
            // req.set("x-harbor-csrf-token",loginAuth.harborToken);
            setAuthorization(req, loginAuth.bearerToken);
        }
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // 发送请求
        beast::http::write(stream, req);
        std::cout << "HTTP request sent." << std::endl;

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }

        // 检查响应状态
        if (res.result() != beast::http::status::ok) {
            std::cerr << "HTTP request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
            return;
        }

        // for (auto const& field : res) {
        //     std::cout << field.name_string() << ": " << field.value() << "\n";
        // }

        // 输出响应体到文件
        std::ofstream ofs(output_tmp, std::ios::binary); // 打开文件为二进制模式
        if (!ofs) {
            std::cerr << "Failed to open file for writing: " << output_tmp << std::endl;
            return;
        }

        ofs << res.body(); // 将响应体写入文件
        ofs.close();
        std::cout << "Blob saved to: " << output_tmp << std::endl;
        //校验
        if(isCorrect(shaId,output_tmp)){
            // 写blob
            std::ofstream ofs1(output_file, std::ios::binary); // 打开文件为二进制模式
            if (!ofs1) {
                std::cerr << "Failed to open file for writing: " << output_file << std::endl;
                return;
            }

            ofs1 << res.body(); // 将响应体写入文件
            ofs1.close();
            std::cout << "Blob saved to: " << output_file << std::endl;
        }

        if(std::remove(output_tmp.c_str())==0){
            std::cout << "tmp manifest deleted successfully: " << output_tmp << std::endl;
        }else{
            std::cerr << "Failed to delete file: " << output_tmp << std::endl;
        }


    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

bool pullConfig(const std::string& host, const std::string& port,const::string& projectName,const::string& imageName,const std::string digest,const std::string& os,const std::string& arch){
    // std::string target = "/v2/library/busyboximage5/blobs/sha256:f28efabc598d38f0b7cea1641bd20b097b8c5aaf090035d7167370608d86fb67"; // API v2 路径
    std::string target= "/v2/"+projectName+"/"+imageName+"/blobs/"+digest;
    std::string output_folder = "oci_images/oci_registry/blobs/sha256"; // 保存的文件夹
    std::string shaId=digest.substr(7);
    std::string output_file = output_folder + "/" + shaId; // 完整的文件路径
    std::string output_tmp = "oci_images/tmp/"+shaId; //临时存储 

    try {
        // 确保文件夹存在
        if (!fs::exists(output_folder)) {
            fs::create_directory(output_folder);
            std::cout << "Created directory: " << output_folder << std::endl;
        }

        //如果库中存在这一层数据则直接退出
        if(fs::exists(output_file)){
            return true;
        }

        // IO 服务和解析器
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 设置超时时间
        stream.expires_after(std::chrono::seconds(5)); // 超时 5 秒

        // 解析并连接
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 设置 HTTP 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11); // 使用 GET 请求
        // req.set(beast::http::field::host, host);
        // req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
        // req.set(beast::http::field::cookie,loginAuth.cookie);
        // req.set(beast::http::field::authorization,"Bearer "+loginAuth.bearerToken);
        // req.set("x-harbor-csrf-token",loginAuth.harborToken);
        // req.set(beast::http::field::user_agent, "Boost.Beast/248");
        req.set(beast::http::field::host, host+":"+port);
        // req.set(beast::http::field::cookie,loginAuth.cookie);
        if (loginAuth.bearerToken.empty()) {
            setAuthorization(req, userinfo.username, userinfo.password);
        }
            // req.set(beast::http::field::authorization,"Bearer "+loginAuth.bearerToken);
        else {
            setAuthorization(req, loginAuth.bearerToken);
        }
        // req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-API-Version", "registry/2.0");
        req.set("Accept-Encoding", "gzip");
        req.set(http::field::connection, "close");

        // 发送请求
        beast::http::write(stream, req);
        std::cout << "HTTP request sent." << std::endl;

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }

        // 检查响应状态
        if (res.result() != beast::http::status::ok) {
            std::cerr << "HTTP request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
            return false;
        }

        // for (auto const& field : res) {
        //     std::cout << field.name_string() << ": " << field.value() << "\n";
        // }

        auto config = unmarshal<v1::Image>(res.body());
        //如果os和arch不符合则退出 
        if(config.platform.OS != os || config.platform.Architecture != arch){
            std::cout<<"os or arch not match!"<<"\n";
            return false;
        }

        // 输出响应体到文件
        std::ofstream ofs(output_tmp, std::ios::binary); // 打开文件为二进制模式
        if (!ofs) {
            std::cerr << "Failed to open file for writing: " << output_tmp << std::endl;
            return false;
        }

        ofs << res.body(); // 将响应体写入文件
        ofs.close();
        std::cout << "Blob saved to: " << output_tmp << std::endl;
        //校验
        if(isCorrect(shaId,output_tmp)){
            // 写blob
            std::ofstream ofs1(output_file, std::ios::binary); // 打开文件为二进制模式
            if (!ofs1) {
                std::cerr << "Failed to open file for writing: " << output_file << std::endl;
                return false;
            }

            ofs1 << res.body(); // 将响应体写入文件
            ofs1.close();
            std::cout << "Blob saved to: " << output_file << std::endl;
        }

        if(std::remove(output_tmp.c_str())==0){
            std::cout << "tmp manifest deleted successfully: " << output_tmp << std::endl;
        }else{
            std::cerr << "Failed to delete file: " << output_tmp << std::endl;
        }

        return true;

    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


std::tuple<std::string, size_t> pullManifestAndBlob(const std::string& host, const std::string& port, const std::string& projectName, const std::string& imageName, const std::string& version, const std::string& os, const std::string& arch, bool v1, const std::string& scheme) {
    std::string target = "/v2/" + projectName + "/" + imageName + "/manifests/" + version;
    std::string output_folder = "oci_images/oci_registry/blobs/sha256/"; // 保存的文件夹
    std::string output_file_tmp = "oci_images/tmp/manifest"; // 完整的文件路径

    try {
        // 确保文件夹存在
        if (!fs::exists(output_folder)) {
            fs::create_directories(output_folder);
            std::cout << "Created directory: " << output_folder << std::endl;
        }
        if (!fs::exists("oci_images/tmp/")) {
            fs::create_directories("oci_images/tmp/");
        }

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 设置超时时间
            stream.expires_after(std::chrono::seconds(5)); // 超时 5 秒

            // 解析并连接
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP GET 请求
            http::request<http::empty_body> req(http::verb::get, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::accept,
                "application/vnd.docker.distribution.manifest.v2+json, "
                "application/vnd.oci.image.manifest.v1+json, "
                "application/vnd.docker.distribution.manifest.v1+prettyjws, "
                "application/vnd.docker.distribution.manifest.v1+json, "
                "application/vnd.docker.distribution.manifest.list.v2+json, "
                "application/vnd.oci.image.index.v1+json");
            if (loginAuth.bearerToken.empty()) {
                setAuthorization(req, userinfo.username, userinfo.password);
            } else {
                setAuthorization(req, loginAuth.bearerToken);
            }
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-API-Version", "registry/2.0");
            req.set("Accept-Encoding", "gzip");
            req.set(http::field::connection, "close");

            // 发送请求
            http::write(stream, req);
            std::cout << "HTTP request sent." << std::endl;

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            // 检查响应状态
            if (res.result() != http::status::ok) {
                std::cerr << "HTTP request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
                return {};
            }

            // 提取 X-Harbor-Csrf-Token
            for (auto const& field : res) {
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    loginAuth.harborToken = token;
                    break;
                }
            }

            // 分析 manifest
            auto manifest = unmarshal<::Manifest>(res.body());

            // 检查 os 和 arch 是否符合
            if (!pullConfig(host, port, projectName, imageName, std::string(manifest.Config.Digests.digest), os, arch)) {
                return {};
            }

            // 输出响应体到文件
            std::ofstream ofs(output_file_tmp, std::ios::binary);
            if (!ofs) {
                std::cerr << "Failed to open file for writing: " << output_file_tmp << std::endl;
                return {};
            }
            ofs << res.body();
            ofs.close();

            // 写 manifest
            std::string ManifestSha;
            if (v1 == true) {
                std::string new_path = write_manifest_new(output_file_tmp);
                fs::path source = new_path;
                ManifestSha = source.filename().string();
                fs::path target_path = output_folder + source.filename().string();

                if (fs::exists(source)) {
                    fs::copy_file(source, target_path, fs::copy_option::overwrite_if_exists);
                } else {
                    throw std::runtime_error("Failed to copy file: " + source.string());
                }
            } else {
                // 计算 manifest 的 hash
                ManifestSha = Fromfile(output_file_tmp)->Encoded();
                std::string output_file = output_folder + ManifestSha;

                if (std::remove(output_file_tmp.c_str()) == 0) {
                    // std::cout << "tmp manifest deleted successfully: " << output_file_tmp << std::endl;
                } else {
                    throw std::runtime_error("Failed to delete temp file: " + output_file_tmp);
                }
                std::ofstream ofs1(output_file, std::ios::binary);
                if (!ofs1) {
                    throw std::runtime_error("Failed to open file for writing: " + output_file);
                }
                ofs1 << res.body();
                ofs1.close();
            }

            // 获取 manifest 长度
            size_t manifestLen = 0;
            if (res.find(http::field::content_length) != res.end()) {
                std::string content_length = res[http::field::content_length].to_string();
                manifestLen = std::stoul(content_length);
            } else {
                std::cout << "No Content-Length field found in response." << std::endl;
            }

            // 依次 pull 数据层
            std::vector<Descriptor> Layers = manifest.Layers;
            for (std::size_t i = 0; i < Layers.size(); i++) {
                Descriptor de = Layers[i];
                pullBlob(host, port, projectName, imageName, std::string(de.Digests.digest));
            }

            return std::make_tuple("sha256:" + ManifestSha, manifestLen);
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            ssl::context ctx(ssl::context::tlsv12_client);

            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);

            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }

            // 解析并连接
            auto const results = resolver.resolve(host, port);
            beast::get_lowest_layer(stream).connect(results);

            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构造 HTTP GET 请求
            http::request<http::empty_body> req(http::verb::get, target, 11);
            req.set(http::field::host, host + ":" + port);
            req.set(http::field::accept,
                "application/vnd.docker.distribution.manifest.v2+json, "
                "application/vnd.oci.image.manifest.v1+json, "
                "application/vnd.docker.distribution.manifest.v1+prettyjws, "
                "application/vnd.docker.distribution.manifest.v1+json, "
                "application/vnd.docker.distribution.manifest.list.v2+json, "
                "application/vnd.oci.image.index.v1+json");
            if (loginAuth.bearerToken.empty()) {
                setAuthorization(req, userinfo.username, userinfo.password);
            } else {
                setAuthorization(req, loginAuth.bearerToken);
            }
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set("Docker-Distribution-API-Version", "registry/2.0");
            req.set("Accept-Encoding", "gzip");
            req.set(http::field::connection, "close");

            // 发送请求
            http::write(stream, req);
            std::cout << "HTTP request sent." << std::endl;

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // 关闭连接
            beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            // 检查响应状态
            if (res.result() != http::status::ok) {
                std::cerr << "HTTP request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
                return {};
            }

            // 提取 X-Harbor-Csrf-Token
            for (auto const& field : res) {
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    loginAuth.harborToken = token;
                    break;
                }
            }

            // 分析 manifest
            auto manifest = unmarshal<::Manifest>(res.body());

            // 检查 os 和 arch 是否符合
            if (!pullConfig(host, port, projectName, imageName, std::string(manifest.Config.Digests.digest), os, arch)) {
                return {};
            }

            // 输出响应体到文件
            std::ofstream ofs(output_file_tmp, std::ios::binary);
            if (!ofs) {
                std::cerr << "Failed to open file for writing: " << output_file_tmp << std::endl;
                return {};
            }
            ofs << res.body();
            ofs.close();

            // 写 manifest
            std::string ManifestSha;
            if (v1 == true) {
                std::string new_path = write_manifest_new(output_file_tmp);
                fs::path source = new_path;
                ManifestSha = source.filename().string();
                fs::path target_path = output_folder + source.filename().string();

                if (fs::exists(source)) {
                    fs::copy_file(source, target_path, fs::copy_option::overwrite_if_exists);
                } else {
                    throw std::runtime_error("Failed to copy file: " + source.string());
                }
            } else {
                // 计算 manifest 的 hash
                ManifestSha = Fromfile(output_file_tmp)->Encoded();
                std::string output_file = output_folder + ManifestSha;

                if (std::remove(output_file_tmp.c_str()) == 0) {
                    // std::cout << "tmp manifest deleted successfully: " << output_file_tmp << std::endl;
                } else {
                    throw std::runtime_error("Failed to delete temp file: " + output_file_tmp);
                }
                std::ofstream ofs1(output_file, std::ios::binary);
                if (!ofs1) {
                    throw std::runtime_error("Failed to open file for writing: " + output_file);
                }
                ofs1 << res.body();
                ofs1.close();
            }

            // 获取 manifest 长度
            size_t manifestLen = 0;
            if (res.find(http::field::content_length) != res.end()) {
                std::string content_length = res[http::field::content_length].to_string();
                manifestLen = std::stoul(content_length);
            } else {
                std::cout << "No Content-Length field found in response." << std::endl;
            }

            // 依次 pull 数据层
            std::vector<Descriptor> Layers = manifest.Layers;
            for (std::size_t i = 0; i < Layers.size(); i++) {
                Descriptor de = Layers[i];
                pullBlob(host, port, projectName, imageName, std::string(de.Digests.digest));
            }

            return std::make_tuple("sha256:" + ManifestSha, manifestLen);
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Pull Manifest Exception: " << e.what() << std::endl;
    }
    return {};
}

void getCookieFromAuthFile(){
    std::string cookieFile = "oci_images/auth.json";
    std::ifstream ifs(cookieFile);
    if (ifs) {
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        ifs.close(); // 显式关闭文件

        try {
            json::value jsonData = json::parse(buffer.str());
            if (jsonData.is_object()) {
                json::object obj = jsonData.as_object();
                if (obj.contains("cookie")) {
                    std::string cookie = obj["cookie"].as_string().c_str();
                    // std::cout << "Cookie loaded from JSON: " << cookie << "\n";
                    loginAuth.cookie = cookie;
                }
            }
            std::cerr << "No cookie found in JSON file.\n";
        } catch (const std::exception& e) {
            std::cerr << "Error parsing JSON: " << e.what() << "\n";
        }
    } else {
        std::cerr << "Failed to open file for loading cookie.\n";
    }
}

void saveLoginInfo(const std::string& username, const std::string& password, const std::string& ipAddr){
    std::string authPath = "oci_images/auth.json";
    if (!boost::filesystem::exists("oci_images")) {
        boost::filesystem::create_directories("oci_images");
    }
    json::object credentials;
    credentials["username"] = username;
    credentials["password"] = password;
    credentials["ipAddr"] = ipAddr;
    boost::json::value jv = credentials;
    format_json(jv, 4);

    std::ofstream ofs(authPath);
    if (ofs) {
        ofs << json::serialize(credentials);
        std::cout << "Credentials saved to auth.json\n";
    } else {
        std::cerr << "Failed to save credentials\n";
    }
}

void loadLoginInfo(std::string ipAddr) {
    std::string authPath = "oci_images/auth.json";

    std::ifstream ifs(authPath);
    if (!ifs) {
        std::cerr << "Failed to open auth.json for reading\n";
        return;
    }

    try {
        // 读取文件内容
        std::ostringstream oss;
        oss << ifs.rdbuf();
        std::string content = oss.str();

        // 解析 JSON 对象
        json::value parsed = json::parse(content);
        json::object credentials = parsed.as_object();
        std::string username;
        std::string password;
        if (credentials.find(ipAddr) != credentials.end()) {
            user usr = json::value_to<user>(credentials[ipAddr]);
            userinfo.username=usr.username;
            userinfo.password=usr.password;
        } else {
            std::cerr << "auth.json does not contain required fields\n";
        }
        // // 提取字段
        // if (credentials.contains("username") && credentials.contains("password")) {
        //     username = json::value_to<std::string>(credentials["username"]);
        //     password = json::value_to<std::string>(credentials["password"]);
        //     userinfo.username=username;
        //     userinfo.password=password;
        // } else {
        //     std::cerr << "auth.json does not contain required fields\n";
        // }
    } catch (const std::exception& e) {
        std::cerr << "Error reading auth.json: " << e.what() << "\n";
    }
}


std::vector<std::string> getTagList(const std::string& host, const std::string& port, const std::string& projectName, const std::string& imageName, const std::string& scheme) {
    try {
        std::string target = "/v2/" + projectName + "/" + imageName + "/tags/list";

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);

        // 根据协议选择不同的流类型
        if (scheme == "http") {
            // 创建 TCP 流
            beast::tcp_stream stream(ioc);

            // 解析并连接到主机
            auto const results = resolver.resolve(host, port);
            stream.connect(results);

            // 构造 HTTP GET 请求
            http::request<http::empty_body> req(http::verb::get, target, 11);
            req.set(http::field::host, host);
            req.set(http::field::cookie, loginAuth.cookie);
            req.set(http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
            req.set("x-harbor-csrf-token", loginAuth.harborToken);
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            // 检查响应状态
            if (res.result() != http::status::ok) {
                std::cerr << "GetTagsList request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
                return {};
            }

            // 提取 X-Harbor-Csrf-Token
            for (auto const& field : res) {
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    loginAuth.harborToken = token;
                    break;
                }
            }

            // 解析响应体中的 JSON 数据
            std::vector<std::string> tags;
            json::value json_value = json::parse(res.body());
            json::object json_obj = json_value.as_object();

            // 检查 "tags" 是否存在并提取它们
            if (json_obj.contains("tags")) {
                auto tags_array = json_obj["tags"].as_array();
                for (const auto& tag : tags_array) {
                    tags.push_back(tag.as_string().c_str());
                }
            }

            return tags;
        } else if (scheme == "https") {
            // 创建 SSL 上下文
            ssl::context ctx(ssl::context::tlsv12_client);

            // 创建 SSL 流
            ssl::stream<beast::tcp_stream> stream(ioc, ctx);

            // 设置 SNI 主机名
            if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
                throw boost::system::system_error(
                    boost::system::error_code(
                        static_cast<int>(::ERR_get_error()),
                        boost::asio::error::get_ssl_category()));
            }

            // 解析并连接
            auto const results = resolver.resolve(host, port);
            beast::get_lowest_layer(stream).connect(results);

            // SSL 握手
            stream.handshake(ssl::stream_base::client);

            // 构造 HTTP GET 请求
            http::request<http::empty_body> req(http::verb::get, target, 11);
            req.set(http::field::host, host);
            req.set(http::field::cookie, loginAuth.cookie);
            req.set(http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
            req.set("x-harbor-csrf-token", loginAuth.harborToken);
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

            // 发送请求
            http::write(stream, req);

            // 接收响应
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // 关闭连接
            beast::get_lowest_layer(stream).socket().shutdown(asio::ip::tcp::socket::shutdown_both);

            // 检查响应状态
            if (res.result() != http::status::ok) {
                std::cerr << "GetTagsList request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
                return {};
            }

            // 提取 X-Harbor-Csrf-Token
            for (auto const& field : res) {
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    loginAuth.harborToken = token;
                    break;
                }
            }

            // 解析响应体中的 JSON 数据
            std::vector<std::string> tags;
            json::value json_value = json::parse(res.body());
            json::object json_obj = json_value.as_object();

            // 检查 "tags" 是否存在并提取它们
            if (json_obj.contains("tags")) {
                auto tags_array = json_obj["tags"].as_array();
                for (const auto& tag : tags_array) {
                    tags.push_back(tag.as_string().c_str());
                }
            }

            return tags;
        } else {
            throw std::invalid_argument("Unsupported scheme: " + scheme);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return {};
    }
}