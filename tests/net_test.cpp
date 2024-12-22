#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <string>
#include "network/network.h"
namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
using tcp = asio::ip::tcp;

// void login(const std::string& user, const std::string& passwd) {
//     // 设置认证服务器的URL和请求头
//     std::string host = "192.168.182.128"; // Harbor 服务器地址
//     std::string port = "80"; // 端口号
//     std::string target = "/c/login";  // 登录接口路径
//     const int version = 11; // HTTP 1.1

//     // 使用 Boost.Asio 和 Beast 发起 HTTP 请求
//     asio::io_context ioc;
//     beast::tcp_stream stream(ioc);
//     tcp::resolver resolver(ioc);

//     // 解析主机和端口
//     auto const results = resolver.resolve(host, port);

//     // 连接到服务器
//     stream.connect(results);

//     // 发送一个 GET 请求，获取 CSRF 令牌
//     beast::http::request<beast::http::empty_body> req_get;
//     req_get.method(beast::http::verb::get);  // 使用 GET 方法
//     req_get.target("/api/v2.0/systeminfo");  
//     req_get.set(beast::http::field::host, host);  // 设置 Host 头
//     req_get.set(beast::http::field::connection, "close");  // 请求完成后关闭连接

//     // 发送 GET 请求
//     beast::http::write(stream, req_get);

//     // 接收响应
//     beast::flat_buffer buffer;
//     beast::http::response<beast::http::string_body> res_get;
//     beast::http::read(stream, buffer, res_get);

//     for (auto const& field : res_get) {
//         std::cout << field.name_string() << ": " << field.value() << "\n";
//     }
//     // 提取 CSRF 令牌（从 Set-Cookie 头中获取）
//     std::string csrf_token;
//     for (auto const& field : res_get) {
//         if (field.name_string() == "Set-Cookie") {
//             std::string cookie = field.value().to_string();
//             // 这里是简单的解析方式，实际可能需要正则表达式或更复杂的解析
//             std::size_t pos = cookie.find("_gorilla_csrf=");
//             if (pos != std::string::npos) {
//                 csrf_token = cookie.substr(pos + 15, cookie.find(";") - pos - 15);
//                 break;
//             }
//         }
//     }

//     // 打印提取的 CSRF 令牌
//     std::cout << "Extracted CSRF Token: " << csrf_token << std::endl;

//     // 创建登录请求体，包含用户名和密码
//     std::stringstream body;
//     body << R"({"principal":")" << user << R"(","password":")" << passwd << R"("})";

//     // 设置 HTTP 请求
//     beast::http::request<beast::http::string_body> req_post;
//     req_post.method(beast::http::verb::post);  // 使用 POST 方法
//     req_post.target(target);  // 设置目标路径
//     req_post.set(beast::http::field::host, host);  // 设置 Host 头
//     req_post.set(beast::http::field::content_type, "application/json");  // 设置 Content-Type
//     req_post.set(beast::http::field::content_length, std::to_string(body.str().size()));  // 设置请求体大小
//     req_post.set(beast::http::field::connection, "close");  // 请求完成后关闭连接
//     req_post.body() = body.str();  // 设置请求体
//     req_post.prepare_payload();  // 准备请求体

//     // 设置 CSRF 令牌（从 GET 请求中提取）
//     req_post.set("X-Harbor-Csrf-Token", csrf_token);

//     // 发送 POST 请求
//     beast::http::write(stream, req_post);

//     // 接收响应
//     beast::http::response<beast::http::string_body> res_post;
//     beast::http::read(stream, buffer, res_post);

//     // 打印响应内容
//     std::cout << "Response received.\n";
//     std::cout << "HTTP Version: " << (res_post.version() / 10) << "." << (res_post.version() % 10) << "\n";
//     std::cout << "Status: " << res_post.result_int() << " " << res_post.reason() << "\n";
//     std::cout << "Headers:\n";
//     for (auto const& field : res_post) {
//         std::cout << field.name_string() << ": " << field.value() << "\n";
//     }

//     // 打印响应体（可能是 JSON 格式，包含 token 或错误信息）
//     std::cout << "\nResponse Body:\n";
//     std::cout << res_post.body() << "\n";

//     // 关闭连接
//     stream.close();
// }

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


std::pair<std::string,std::string> getToken(const std::string& host, const std::string& port,const std::string& cookie){
    std::string target="/service/token?service=harbor-registry&scope=repository:library/busybox:pull";

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

    std::string harbor_csrf_token;
    for (auto const& field : res) {
        if (field.name_string() == "X-Harbor-Csrf-Token") {
            std::string token = field.value().to_string();
            std::cout<<"h_Token: "<<token<<"\n\n";
            harbor_csrf_token = token;
            break;
        }
    }
    std::cout<<res.body()<<"\n";
    std::string token=extractToken(res.body());
    return {harbor_csrf_token,token};

}

// 获取认证令牌
void login(const std::string& user, const std::string& passwd) {

    // 设置认证服务器的URL和请求头
    std::string host = "192.168.182.128";
    std::string port = "80";
    std::string target = "/c/login";  
    const int version = 11; // HTTP 1.1
    std::string body = "principal=" + user + "&password=" + passwd;


    // 使用Boost.Asio和Beast发起HTTP POST请求
    asio::io_context ioc;
    beast::tcp_stream stream(ioc);
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, port);

    // 连接服务器
    stream.connect(results);

    // 设置HTTP请求
    beast::http::request<beast::http::string_body> req;
    req.method(beast::http::verb::post);
    req.target(target);
    req.set(beast::http::field::host, host);
    req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
    req.set(beast::http::field::content_length,std::to_string(body.size()));
    req.set(beast::http::field::user_agent, "Boost.Beast/248");
    // req.set(beast::http::field::connection, "close");
    req.body() = body;
    req.prepare_payload();
    std::cout<<"content_Length: "<<std::to_string(body.size())<<"\n";
    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> res;
    beast::http::read(stream, buffer, res);
    std::cout << "Response received." << std::endl;

    // // 打印响应内容
    // std::cout << "HTTP Version: " << (res.version() / 10) << "." << (res.version() % 10) << "\n";
    // std::cout << "Status: " << res.result_int() << " " << res.reason() << "\n";
    // std::cout << "Headers:\n";
    for (auto const& field : res) {
        std::cout << field.name_string() << ": " << field.value() << "\n";
    }
    std::cout << res.result() << "\n";
    std::cout << "\nResponse Body:\n";
    std::cout << res.body() << "\n";

    if(res.result() == beast::http::status::forbidden){
        // 提取 CSRF 令牌（从 Set-Cookie 头中获取）
        std::string csrf_token;
        std::string harbor_csrf_token;
        for (auto const& field : res) {
             if (field.name_string() == "Set-Cookie") {
                std::string cookie = field.value().to_string();
                // 查找 _gorilla_csrf= 的位置
                std::size_t start_pos = cookie.find("_gorilla_csrf=");
                if (start_pos != std::string::npos) {
                    start_pos += 14; // 跳过 "_gorilla_csrf=" 的长度
                    // 从 start_pos 开始查找下一个分号
                    std::size_t end_pos = cookie.find(";", start_pos);
                    if (end_pos == std::string::npos) {
                        end_pos = cookie.length(); // 如果没有分号，则取到字符串结尾
                    }
                    csrf_token = cookie.substr(start_pos, end_pos - start_pos);
                }
            }
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                harbor_csrf_token = token;
            }
        }

        if (!csrf_token.empty()) {
            std::cout << "Extracted CSRF Token: " << csrf_token << std::endl;
            std::cout << "Extracted harbor_csrf_token Token: " << harbor_csrf_token << std::endl;   
            // 重新建立连接用于发送带有 CSRF token 的请求
            beast::tcp_stream new_stream(ioc);
            new_stream.connect(results);

            std::string cookie = "_gorilla_csrf=" + csrf_token + ";";

            beast::http::request<beast::http::string_body> req1;
            req1.method(beast::http::verb::post);
            req1.target(target);
            req1.set(beast::http::field::host, host);
            req1.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
            req1.set(beast::http::field::content_length, std::to_string(body.size()));
            req1.set(beast::http::field::cookie, cookie);
            req1.set(beast::http::field::user_agent, "Boost.Beast/248");
            req1.set("x-harbor-csrf-token",harbor_csrf_token);
            req1.body() = body;
            req1.prepare_payload();

            // 发送带有 CSRF token 的请求
            beast::http::write(new_stream, req1);

            // 接收响应
            beast::flat_buffer buffer1;
            beast::http::response<beast::http::string_body> res1;
            beast::http::read(new_stream, buffer1, res1);
            std::cout << "Response received." << std::endl;

            for (auto const& field : res1) {
                std::cout << field.name_string() << ": " << field.value() << "\n";
            }

            std::cout << res1.result() << "\n";
            std::cout << "\nResponse Body:\n";
            std::cout << res1.body() << "\n";
        }

    }

    // 关闭连接
    stream.close();
}

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

        // login("admin","Harbor12345");
        std::string cookie = "_gorilla_csrf=MTczNDg3Mjg0NXxJbUpKZGxFNUwzQm1RamRGZW5GRlNrSm5aV1ptT0dwREwzcHFhemRqV2tSeFVXYzROWGx5Vm05eE5UQTlJZ289fLAperNqJGnO0kWux4zpdo3rNHhL5sQYRXjw3csgriq7;sid=238f99d6c796386af39196d230d82790;";
        // ifSupportV2(cookie);
        ifBlobExists(cookie);
        // getToken("192.168.182.128","80",cookie);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
