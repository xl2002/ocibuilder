#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <string>

#include <boost/regex.hpp>

// #include "network/network.h"
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
namespace http = boost::beast::http;
using tcp = asio::ip::tcp;


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
            std::cout<<token<<"\n";
            return token;
        } else {
            return "";
        }
    } else {
        return "";
    }
}

std::string extractTokentest(const std::string& response_body){
    // 查找 "token" 字段的位置
    std::string token_key = "token";
    size_t token_pos = response_body.find(token_key);
    
    if (token_pos != std::string::npos) {
        // 提取 token 字段
        size_t start = token_pos + token_key.length();
        size_t end = response_body.find("\"", start);
        
        if (end != std::string::npos) {
            std::string token = response_body.substr(start, end - start);
            std::cout<<token<<"\n";
            return token;
        } else {
            return "";
        }
    } else {
        return "";
    }
}


std::pair<std::string,std::string> getToken(const std::string& host, const std::string& port,const std::string& cookie){
    std::string target="/service/token?service=harbor-registry&scope=repository:library/busybox:pull";
    // std::string target="/service/token?service=harbor-registry";

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
    // req.set(beast::http::field::cookie,cookie);
    req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> res;
    beast::http::read(stream, buffer, res);

    // 打印响应内容
    std::cout << "GetToken!!!";
    std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
    std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
    std::cout << "Headers:\n";
    for (auto const& field : res) {
        std::cout << field.name_string() << ": " << field.value() << "\n";
    }
    for (auto const& field : res) {
        std::cout << field.name_string() << "\n";
    }

    std::string harbor_csrf_token;
    for (auto const& field : res) {
        if (field.name_string() == "X-Harbor-Csrf-Token") {
            std::string token = field.value().to_string();
            // std::cout<<"h_Token: "<<token<<"\n\n";
            harbor_csrf_token = token;
            break;
        }
    }
    // std::cout<<res.body()<<"\n";
    std::string token=extractToken(res.body());
    return {harbor_csrf_token,token};

}

std::string base64_encode(const std::string &in)
{
    std::string out;
    out.resize(beast::detail::base64::encoded_size(in.size()));
    beast::detail::base64::encode(&out[0], in.data(), in.size());
    return out;
}

void login(const std::string& user, const std::string& passwd){
    try {
        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
        std::string host = "192.168.1.102";
        std::string port = "80";
        std::string target = "/v2/";  // 根据需要修改路径
        int version = 11;  // HTTP 版本，1.1

        // std::string body = "principal=" + user + "&password=" + passwd;
        
        // 创建 TCP 解析器
        asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);

        // 创建 socket
        beast::tcp_stream stream(ioc);
        stream.connect(results);

        // 构建 HTTP 请求
        http::request<http::empty_body> req{http::verb::get, target, version};
        // 设置其他头部信息
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, "Boost.Beast/248");
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
            std::string csrf_token;
            std::string harbor_csrf_token;
            bool receive_cookie = false;
            // 获取cookie的长度
            for (auto const& field : res) {
                if (field.name_string() == "Set-Cookie") {
                    receive_cookie = true;
                }
            }
            // 关闭连接
            beast::error_code ec;
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            // 如果接受到cookie，说明是harbor方式，需要继续发送请求
            if (receive_cookie) {
                // 创建新的输入流
                beast::tcp_stream stream2(ioc);
                stream2.connect(results);

                // std::string target2 = "/service/token?account=admin&scope=repository%3Alibrary%2Fbusybox%3Apull%2Cpush&service=harbor-registry";
                std::string target2 = "/service/token?account=admin&service=harbor-registry";
                http::request<http::empty_body> req2{http::verb::get, target2, version};
                // 设置用户的登录信息
                std::string credentials = user + ":" + passwd;
                std::string encoded_credentials = base64_encode(credentials);
                req2.set(http::field::authorization, "Basic " + encoded_credentials);

                req2.set(http::field::host, host+":"+port);
                req2.set(http::field::user_agent, "Boost.Beast/248");
                req2.set("Accept-Encoding", "gzip");
                req2.set(http::field::connection, "close");
                http::write(stream2, req2);

                beast::flat_buffer buffer2;
                http::response<http::string_body> res2;
                http::read(stream2, buffer2, res2);

                // 需要从接收到的响应中读取token
                if (res2.result() == http::status::ok) {
                    getToken(host,port,"0");
                    // std::cout << res2.body() << std::endl;
                    std::string btoken=extractTokentest(res2.body());
                    // std::cout << btoken << std::endl;
                }
                beast::get_lowest_layer(stream2).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
                stream2.socket().close();
            }
            
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
// 获取认证令牌

// void login(const std::string& user, const std::string& passwd) {

//     // 设置认证服务器的URL和请求头
//     std::string host = "192.168.1.102";
//     std::string port = "80";
//     std::string target = "/c/login";  
//     const int version = 11; // HTTP 1.1
//     std::string body = "principal=" + user + "&password=" + passwd;


//     // 使用Boost.Asio和Beast发起HTTP POST请求
//     asio::io_context ioc;
//     beast::tcp_stream stream(ioc);
//     tcp::resolver resolver(ioc);
//     auto const results = resolver.resolve(host, port);

//     // 连接服务器
//     stream.connect(results);

//     // 设置HTTP请求
//     beast::http::request<beast::http::string_body> req;
//     req.method(beast::http::verb::post);
//     req.target(target);
//     req.set(beast::http::field::host, host);
//     req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
//     req.set(beast::http::field::content_length,std::to_string(body.size()));
//     req.set(beast::http::field::user_agent, "Boost.Beast/248");
//     // req.set(beast::http::field::connection, "close");
//     req.body() = body;
//     req.prepare_payload();
//     std::cout<<"content_Length: "<<std::to_string(body.size())<<"\n";
//     // 发送请求
//     beast::http::write(stream, req);

//     // 接收响应
//     beast::flat_buffer buffer;
//     beast::http::response<beast::http::string_body> res;
//     beast::http::read(stream, buffer, res);
//     std::cout << "Response received." << std::endl;

//     // // 打印响应内容
//     // std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
//     // std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
//     // std::cout << "Headers:\n";
//     for (auto const& field : res) {
//         std::cout << field.name_string() << ": " << field.value() << "\n";
//     }
//     std::cout << res.result() << "\n";
//     std::cout << "\nResponse Body:\n";
//     std::cout << res.body() << "\n";

//     if(res.result() == beast::http::status::forbidden){
//         // 提取 CSRF 令牌（从 Set-Cookie 头中获取）
//         std::string csrf_token;
//         std::string harbor_csrf_token;
//         for (auto const& field : res) {
//              if (field.name_string() == "Set-Cookie") {
//                 std::string cookie = field.value().to_string();
//                 // 查找 _gorilla_csrf= 的位置
//                 std::size_t start_pos = cookie.find("_gorilla_csrf=");
//                 if (start_pos != std::string::npos) {
//                     start_pos += 14; // 跳过 "_gorilla_csrf=" 的长度
//                     // 从 start_pos 开始查找下一个分号
//                     std::size_t end_pos = cookie.find(";", start_pos);
//                     if (end_pos == std::string::npos) {
//                         end_pos = cookie.length(); // 如果没有分号，则取到字符串结尾
//                     }
//                     csrf_token = cookie.substr(start_pos, end_pos - start_pos);
//                 }
//             }
//             if (field.name_string() == "X-Harbor-Csrf-Token") {
//                 std::string token = field.value().to_string();
//                 harbor_csrf_token = token;
//             }
//         }

//         if (!csrf_token.empty()) {
//             std::cout << "Extracted CSRF Token: " << csrf_token << std::endl;
//             std::cout << "Extracted harbor_csrf_token Token: " << harbor_csrf_token << std::endl;   
//             // 重新建立连接用于发送带有 CSRF token 的请求
//             beast::tcp_stream new_stream(ioc);
//             new_stream.connect(results);

//             std::string cookie = "_gorilla_csrf=" + csrf_token + ";";

//             beast::http::request<beast::http::string_body> req1;
//             req1.method(beast::http::verb::post);
//             req1.target(target);
//             req1.set(beast::http::field::host, host);
//             req1.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
//             req1.set(beast::http::field::content_length, std::to_string(body.size()));
//             req1.set(beast::http::field::cookie, cookie);
//             req1.set(beast::http::field::user_agent, "Boost.Beast/248");
//             req1.set("x-harbor-csrf-token",harbor_csrf_token);
//             req1.body() = body;
//             req1.prepare_payload();

//             // 发送带有 CSRF token 的请求
//             beast::http::write(new_stream, req1);

//             // 接收响应
//             beast::flat_buffer buffer1;
//             beast::http::response<beast::http::string_body> res1;
//             beast::http::read(new_stream, buffer1, res1);
//             std::cout << "Response received." << std::endl;

//             for (auto const& field : res1) {
//                 std::cout << field.name_string() << ": " << field.value() << "\n";
//             }

//             std::string sid;
//             int sidNum=0;
//             for (auto const& field : res1) {
//                 if (field.name_string() == "Set-Cookie") {
//                     std::string cookie = field.value().to_string();
//                     std::size_t sidpos=cookie.find("sid=");
//                     if(sidpos!=std::string::npos){
//                         sidNum++;
//                         if(sidNum==2){
//                             sidpos+=4;
//                             std::size_t end_sid = cookie.find(";", sidpos);
//                             if (end_sid == std::string::npos) {
//                                 end_sid = cookie.length(); // 如果没有分号，则取到字符串结尾
//                             }
//                             sid=cookie.substr(sidpos,end_sid-sidpos);
//                             std::cout<< "The sid:"<<sid<<"\n";
//                             // loginAuth.cookie=cookie+"sid="+sid+";";
//                         }
//                     }  
//                 }
//                 if (field.name_string() == "X-Harbor-Csrf-Token") {
//                     std::string token = field.value().to_string();
//                     harbor_csrf_token = token;
//                 }
//             }

//             std::cout << res1.result() << "\n";
//             std::cout << "\nResponse Body:\n";
//             std::cout << res1.body() << "\n";
//         }

//     }

//     // 关闭连接
//     stream.close();
// }

// 上传文件内容
void upload_file(asio::io_context& ioc, const std::string& token, const std::string& file_path) {
    std::string host = "crpi-jaiqb9leeu8skiop.cn-hangzhou.personal.cr.aliyuncs.com";
    std::string target = "/v2/buildah/busybox/manifests/latest";  // 假设目标URL是此，需根据实际文档调整

    // 读取文件内容
    std::ifstream file(file_path, std::ios::binary);
    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // 使用Boost.Asio和Beast发起HTTP PUT请求上传文件
    beast::tcp_stream stream(ioc);
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, "http");

    // 连接服务器
    stream.connect(results);

    // 设置HTTP请求
    beast::http::request<beast::http::string_body> req;
    req.method(beast::http::verb::put);
    req.target(target);
    req.set(beast::http::field::host, host);
    req.set(beast::http::field::authorization, "Bearer " + token);
    req.set(beast::http::field::content_type, "application/json");
    req.body() = file_content;
    req.prepare_payload();

    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::http::response<beast::http::string_body> res;
    asio::streambuf buffer;
    beast::http::read(stream, buffer, res);  // 修正了调用方式

    // 获取响应数据
    std::cout << "Response: " << boost::asio::buffer_cast<const char*>(buffer.data()) << std::endl;

    // 关闭连接
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);
}

void ifSupportV2(const std::string& cookie){
    try {
        std::cout << "start!!" << std::endl;
        std::string host = "192.168.182.128";  // 替换为实际的 Registry 地址
        std::string target = "/v2/";  // API v2 路径
        int port = 80;  // HTTP 默认端口，若使用 HTTPS，改为 443

        // IO 服务和解析器
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 设置超时时间
        stream.expires_after(std::chrono::seconds(5));  // 超时 5 秒

        // 解析并连接
        auto const results = resolver.resolve(host, std::to_string(port));
        stream.connect(results);

        // 设置 HTTP 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11);  // 发送 HEAD 请求
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,cookie);
        req.set(beast::http::field::user_agent, "Boost.Beast/248");

        // 发送请求
        beast::http::write(stream, req);
        std::cout << "HTTP request sent." << std::endl;

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);
        std::cout << "Response received." << std::endl;

        // 打印响应内容
        std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
        std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
        std::cout << "Headers:\n";
        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        // 检查响应状态码
        if (res.result() == beast::http::status::ok) {
            std::cout << "Registry supports API v2. Response Code: " << res.result_int() << std::endl;
        } else {
            std::cout << "Registry does not support API v2. Response Code: " << res.result_int() << std::endl;
        }

        // 关闭连接
        stream.close();  // 使用 close() 代替 shutdown()
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void ifBlobExists(const std::string& cookie){                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
    try {
        std::string host = "192.168.182.128";  // 替换为实际的 Registry 地址
        // 配置参数
        const std::string target = "/v2/library/busybox/blobs/sha256:f0a3fe58120bd28b27186346f7a7320fdf48665a91d6b76e9d20ffdcac39d15b";
        const std::string port = "80"; // Docker Registry 默认端口

        auto result=getToken(host,port,cookie);
        std::string hToken=result.first;
        std::string token=result.second;



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
        req.set(beast::http::field::cookie,cookie);
        req.set("x-harbor-csrf-token",hToken);
        req.set(beast::http::field::authorization,"Bearer "+token);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        // 打印响应内容
        std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
        std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
        std::cout << "Headers:\n";
        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        // 根据状态码判断 blob 是否存在
        if (res.result() == beast::http::status::ok) {
            std::cout << "Blob exists in the registry.\n";
        } else if (res.result() == beast::http::status::not_found) {

            std::cout << "Blob does not exist in the registry.\n";
            std::cout <<res.body()<<"\n";
        } else {
            std::cout << "Unexpected response: " << res.result_int() << " " << res.reason() << "\n";
            std::cout <<res.body()<<"\n";
        }

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void initUpLoad(){
    try {
        std::string host = "localhost";  // 替换为实际的 Registry 地址
        // 配置参数
        const std::string target = "/v2/busybox/blobs/uploads/";
        const std::string port = "5000"; // Docker Registry 默认端口

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 解析并连接到主机
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 构造 HTTP HEAD 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::post, target, 11);
        req.set(beast::http::field::host,host);
        req.set(beast::http::field::content_length, "0");
        
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        // 打印响应内容
        std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
        std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
        std::cout << "Headers:\n";
        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        std::string location = res[beast::http::field::location].to_string();
        std::string state_param;

        auto pos = location.find("_state=");
        if (pos != std::string::npos) {
            state_param = location.substr(pos + 7);
            std::cout << "State parameter: " << state_param << "\n";
        }


        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}


void resolveRequestURL(std::string path){
    std::size_t colonPos = path.find(':');
    std::size_t slashPos = path.find('/');
    std::size_t lastSlash = path.find_last_of('/');

    if (colonPos == std::string::npos || slashPos == std::string::npos || colonPos > slashPos) {
        throw std::invalid_argument("Invalid path format");
    }

    std::string host = path.substr(0, colonPos);
    std::string portStr = path.substr(colonPos + 1, slashPos - colonPos - 1);

    std::size_t firstSlashAfterHost = path.find('/', colonPos + 1);
    std::size_t secondSlashAfterHost = path.find('/', firstSlashAfterHost + 1);

    std::string projectName;
    std::string imageName;
    std::string version;
    std::size_t colon = path.find(':', lastSlash);

    projectName = path.substr(firstSlashAfterHost + 1, secondSlashAfterHost - firstSlashAfterHost - 1);

    if (colon == std::string::npos) {
        // 没有 ':'，说明没有 tag，直接返回 '/' 后的部分
        imageName=path.substr(lastSlash + 1);
        version="latest";
    } else {
        // 有 ':'，返回 ':' 之前的部分
        imageName=path.substr(lastSlash + 1, colon - lastSlash - 1);
        version=path.substr(colon+1);
    }
    std::cout<<"HOST: "<<host<<"\n";
    std::cout<<"port: "<<portStr<<"\n";
    std::cout<<"projectName: "<<projectName<<"\n";
    std::cout<<"imageName: "<<imageName<<"\n";
    std::cout<<"version: "<<version<<"\n";

}

int main() {
    try {
        // // 初始化IO上下文
        // asio::io_context ioc;

        // // 用户凭证
        // std::string user = "alyxk";
        // std::string passwd = "xk1223456";

        // // 获取认证令牌
        // std::string token = get_auth_token(ioc, user, passwd);
        // if (token.empty()) {
        //     std::cerr << "Failed to get authentication token" << std::endl;
        //     return 1;
        // }
        // // 获取认证令牌
        // std::string token = get_auth_token(ioc, user, passwd);
        // if (token.empty()) {
        //     std::cerr << "Failed to get authentication token" << std::endl;
        //     return 1;
        // }

        // std::cout << "Auth token: " << token << std::endl;
        // std::cout << "Auth token: " << token << std::endl;

        // // 上传文件到仓库
        // upload_file(ioc, token, "manifest.json");
        // // 上传文件到仓库
        // upload_file(ioc, token, "manifest.json");

        // std::cout << "File uploaded successfully!" << std::endl;
        // ifBlobExists();
        // initUpLoad();

        // auto url = std::make_shared<URL>();
        // dockerClient client;
        // url=client.resolveRequestURL("localhost:5111/bbbb:123");
        // std::cout<<"host"<<url->host<<"\n";
        // std::cout<<"port"<<url->port<<"\n";
        // std::cout<<"ImageName"<<url->imageName<<"\n";

        login("admin","Harbor12345");
        // std::string cookie = "_gorilla_csrf=MTczNDg3Mjg0NXxJbUpKZGxFNUwzQm1RamRGZW5GRlNrSm5aV1ptT0dwREwzcHFhemRqV2tSeFVXYzROWGx5Vm05eE5UQTlJZ289fLAperNqJGnO0kWux4zpdo3rNHhL5sQYRXjw3csgriq7;sid=238f99d6c796386af39196d230d82790;";
        // ifSupportV2(cookie);
        // ifBlobExists(cookie);
        // getToken("192.168.182.128","80",cookie);

        // resolveRequestURL("localhost:5000/p/busybox:34.1");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
