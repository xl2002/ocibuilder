#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <fstream>
#include <string>

#include <boost/regex.hpp>
// 用来解压GZIP
#include <zlib.h>

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

// login最后需要获取一个token，如果有这个token，则将其嵌入到authroization中，否则authorization是用户名和密码
std::string login_and_getToken(const std::string& user, const std::string& passwd){
    try {
        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
        std::string host = "192.168.1.102";
        std::string port = "80";
        std::string target = "/v2/";  // 根据需要修改路径
        int version = 11;  // HTTP 版本，1.1
        
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
                // 第二次发送请求
                beast::tcp_stream stream2(ioc);
                stream2.connect(results);

                std::string target2 = "/service/token?account=admin&service=harbor-registry";
                http::request<http::empty_body> req2{http::verb::get, target2, version};
                // 设置用户的登录信息
                setAuthorization(req2, user, passwd);
                req2.set(http::field::host, host+":"+port);
                req2.set(http::field::user_agent, "Boost.Beast/248");
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
                    if (!is_gzip) {
                        btoken = extractToken(res2.body());
                    }
                    return btoken;
                }
                beast::get_lowest_layer(stream2).socket().shutdown(asio::ip::tcp::socket::shutdown_both);
                stream2.socket().close();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return "";
}

bool login(std::string user, std::string passwd, std::string token) {
    try {
        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
        std::string host = "192.168.1.102";
        std::string port = "80";
        std::string target = "/v2/";  // 根据需要修改路径
        int version = 11;  // HTTP 版本，1.1
        
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
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set("Accept-Encoding", "gzip");
        req.set(http::field::connection, "close");        
        if (!token.empty()) {
            setAuthorization(req, token);
        } else {
            setAuthorization(req, user, passwd);
        }
        http::write(stream, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);
        if (res.result() == http::status::ok) {
            std::cout << "Login success!!" << std::endl;
            return true;
        } else {
            std::cerr << "Login failed!!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return false;
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

// 测试acore库
void ifBlobExists(const std::string& cookie){
    try {
        std::string host = "192.168.1.102";  // 替换为实际的 Registry 地址
        int version = 11;
        // 配置参数
        // const std::string target = "/v2/library/image3/blobs/sha256:78ef48b3f2044092ab1e6c562cedfda177321db5351559993cd50d0667416b2d";
        const std::string target = "/v2/library/imagetest1/blobs/sha256:83e39b496d70db94e0c30afb3beba1b25dc7e4f7b025fd79e79bd25490019eca";
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
        
        // beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11);
        http::request<http::empty_body> req{http::verb::head, target, version};
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, "Boost.Beast/248");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        std::string credentials = "admin:123456";
        std::string encoded_credentials = base64_encode(credentials);
        req.set(http::field::authorization, "Basic " + encoded_credentials);
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

void initUpLoad(std::string &uid, std::string &state_param){
    try {
        std::string host = "192.168.1.102";  // 替换为实际的 Registry 地址
        // 配置参数
        const std::string target = "/v2/library/image3/blobs/uploads/";
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
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, "Boost.Beast/248");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        req.set(http::field::accept_encoding, "gzip");
        setAuthorization(req, "admin", "123456");

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        if (beast::http::status::ok == res.result()) {
            std::cout << "ok" << std::endl;
        }

        // 打印响应内容
        for (auto const& field : res) {
            if (field.name_string() == "Docker-Upload-Uuid") {
                uid = field.value().to_string();
                std::cout << "Docker-Upload-UUID: " << uid << "\n";
            }
        }

        std::string location = res[beast::http::field::location].to_string();
        // std::string state_param;

        auto pos = location.find("?state_=");
        if (pos != std::string::npos) {
            state_param = location.substr(pos + 8);
            std::cout << "State parameter: " << state_param << "\n";
        }


        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

std::string uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
    const std::string& file_path, std::size_t start, std::size_t end, std::size_t total_size,const std::string& imageName,const std::string& projectName,
    std::string &new_uid, std::string& new_state) {
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


    asio::io_context ioc;
    tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    auto const results = resolver.resolve(host, port);
    stream.connect(results);

    // std::string target = "/v2/" + projectName + "/"+imageName + "/blobs/upload/" + uid + "?state_=" + state;
    std::string target = "/library/" + projectName + "/blobs/upload/" + uid + "?state_=" + state;
    beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
    req.set(beast::http::field::host, host + ":" + port);
    // req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
    req.set(http::field::user_agent, "Boost.Beast/248");
    // transfer_encoding
    std::string credentials = "admin:123456";
    std::string encoded_credentials = base64_encode(credentials);
    req.set(http::field::authorization, "Basic " + encoded_credentials);
    req.set(beast::http::field::content_type, "application/octet-stream");
    req.set("Docker-Distribution-Api-Version", "registry/2.0");
    req.set(http::field::accept_encoding, "gzip");
    req.set(http::field::connection, "close");

    // req.set(beast::http::field::content_range, "bytes=" + std::to_string(start) + "-" + std::to_string(end - 1) + "/" + std::to_string(total_size));
    // req.set(beast::http::field::content_length, std::to_string(bytes_read));
    

    req.body().data = data.data();
    req.body().size = bytes_read; 
    req.body().more=false;
    req.prepare_payload();

    beast::http::write(stream, req);

    beast::flat_buffer buffer_r;
    beast::http::response<beast::http::string_body> res;
    beast::http::read(stream, buffer_r, res);
    stream.socket().shutdown(tcp::socket::shutdown_both);

    if (res.result() != beast::http::status::accepted) {
        throw std::runtime_error("Failed to upload blob chunk");
    }

    std::string location = res[beast::http::field::location].to_string();
    for (auto const& field : res) {
        if (field.name_string() == "Docker-Upload-Uuid") {
            new_uid = field.value().to_string();
            std::cout << "New Docker-Upload-UUID: " << new_uid << "\n";
        }
    }
    auto pos1 = location.find("state_=");
    if (pos1 != std::string::npos) {
        new_state = location.substr(pos1 + 7);
        std::cout << "State parameter: " << new_state << "\n";
    }
    return new_state;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return state;
    }
}

void finalizeUpload(const std::string& host, const std::string& port, const std::string& uid, const std::string& shaId, const std::string& state,const std::string& imageName,const std::string& projectName) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/" +projectName + "/"+ imageName + "/blobs/upload/" + uid + "?_state=" + state + "&digest=sha256:" + shaId;
        beast::http::request<beast::http::empty_body> req(beast::http::verb::put, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, "Boost.Beast/248");
        req.set(beast::http::field::content_type, "application/octet-stream");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        req.set(http::field::accept_encoding, "gzip");
        std::string credentials = "admin:123456";
        std::string encoded_credentials = base64_encode(credentials);
        req.set(http::field::authorization, "Basic " + encoded_credentials);

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to finalize upload");
        }
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
        std::string token = login_and_getToken("admin", "Harbor12345");
        std::cout << token << std::endl;
        std::cout << login("admin", "Harbor12345", token) << std::endl;
        // ifBlobExists("123");
        // std::string uid, state;
        // initUpLoad(uid, state);
        // std::string filePath = "tests/83e39b496d70db94e0c30afb3beba1b25dc7e4f7b025fd79e79bd25490019eca";
        // std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        // if (!file)
        //     throw std::runtime_error("Failed to open file: " + filePath);
        // std::size_t total_size = file.tellg();
        // file.close();
        // std::string new_uid, new_state;
        // uploadBlobChunk("192.168.1.102", "5000", uid, state, filePath, 0, total_size, total_size, "image3", "image3", new_uid, new_state);
        // std::string shaID = "83e39b496d70db94e0c30afb3beba1b25dc7e4f7b025fd79e79bd25490019eca";
        // finalizeUpload("192.168.1.102", "5000", new_uid, shaID, new_state, "image3", "library");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
