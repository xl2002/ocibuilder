#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
using tcp = asio::ip::tcp;

// 获取认证令牌
std::string get_auth_token(asio::io_context& ioc, const std::string& user, const std::string& passwd) {
    std::string token;

    // 设置认证服务器的URL和请求头
    std::string host = "crpi-jaiqb9leeu8skiop.cn-hangzhou.personal.cr.aliyuncs.com";
    std::string target = "/v2/token";  // 假设认证URL是此，需根据实际文档调整
    std::string auth_data = "grant_type=client_credentials&client_id=" + user + "&client_secret=" + passwd;

    // 使用Boost.Asio和Beast发起HTTP POST请求
    beast::tcp_stream stream(ioc);
    tcp::resolver resolver(ioc);
    auto const results = resolver.resolve(host, "http");

    // 连接服务器
    stream.connect(results);

    // 设置HTTP请求
    beast::http::request<beast::http::string_body> req;
    req.method(beast::http::verb::post);
    req.target(target);
    req.set(beast::http::field::host, host);
    req.set(beast::http::field::content_type, "application/x-www-form-urlencoded");
    req.body() = auth_data;
    req.prepare_payload();

    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::http::response<beast::http::string_body> res;
    asio::streambuf buffer;
    beast::http::read(stream, buffer, res);  // 修正了调用方式

    // 获取响应数据
    std::string res_body = res.body();

    // 关闭连接
    beast::error_code ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, ec);

    // 解析JSON响应，提取token
    json::value jv = json::parse(res_body);
    if (jv.as_object().contains("access_token")) {
        token = jv.as_object()["access_token"].as_string().c_str();
    }

    return token;
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

int main() {
    try {
        // 初始化IO上下文
        asio::io_context ioc;

        // 用户凭证
        std::string user = "alyxk";
        std::string passwd = "xk1223456";

        // 获取认证令牌
        std::string token = get_auth_token(ioc, user, passwd);
        if (token.empty()) {
            std::cerr << "Failed to get authentication token" << std::endl;
            return 1;
        }

        std::cout << "Auth token: " << token << std::endl;

        // 上传文件到仓库
        upload_file(ioc, token, "manifest.json");

        std::cout << "File uploaded successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
