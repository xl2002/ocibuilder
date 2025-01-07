#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace json = boost::json;
namespace fs = boost::filesystem;
using tcp = asio::ip::tcp;

// 登录
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

            std::string sid;
            int sidNum=0;
            for (auto const& field : res1) {
                if (field.name_string() == "Set-Cookie") {
                    std::string cookie = field.value().to_string();
                    std::size_t sidpos=cookie.find("sid=");
                    if(sidpos!=std::string::npos){
                        sidNum++;
                        if(sidNum==2){
                            sidpos+=4;
                            std::size_t end_sid = cookie.find(";", sidpos);
                            if (end_sid == std::string::npos) {
                                end_sid = cookie.length(); // 如果没有分号，则取到字符串结尾
                            }
                            sid=cookie.substr(sidpos,end_sid-sidpos);
                            std::cout<< "The sid:"<<sid<<"\n";
                            // loginAuth.cookie=cookie+"sid="+sid+";";
                        }
                    }  
                }
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    harbor_csrf_token = token;
                }
            }

            std::cout << res1.result() << "\n";
            std::cout << "\nResponse Body:\n";
            std::cout << res1.body() << "\n";
        }

    }

    // 关闭连接
    stream.close();
}

void pullManifest(const std::string& host,const std::string& port){
    // 设置目标路径和文件夹
    std::string target = "/v2/library/busyboximage5/blobs/sha256:f28efabc598d38f0b7cea1641bd20b097b8c5aaf090035d7167370608d86fb67"; // API v2 路径
    std::string output_folder = "manifests/blobs"; // 保存的文件夹
    std::string output_file = output_folder + "/blob3"; // 完整的文件路径

    try {
        // 确保文件夹存在
        if (!fs::exists(output_folder)) {
            fs::create_directory(output_folder);
            std::cout << "Created directory: " << output_folder << std::endl;
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
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
        req.set(beast::http::field::cookie,"_gorilla_csrf=MTczNTQ1NjQ0OHxJbUV3ZWt0MGNIRldOVEY2TUROMVNTOWxRM0JNTnpCUFJVVkdkblZLUWtaNVdrSnRWM050U3pFemMwMDlJZ289fFvtJxOEdkC7vMGjHJs3eYO4cyYakQ3PKvM_bM7wUBuR;sid=bba13b18e047dc060d237fcaac9f743a;");
        req.set(beast::http::field::authorization,"Bearer eyJhbGciOiJSUzI1NiIsImtpZCI6IlFRVzY6TlVWWjpTWTZPOjJYUEw6R05KNTpBVFJGOjdGTzQ6SU5RSzpBM0pDOkNDSE46VDJHRjpFSUpaIiwidHlwIjoiSldUIn0.eyJpc3MiOiJoYXJib3ItdG9rZW4taXNzdWVyIiwic3ViIjoiYWRtaW4iLCJhdWQiOiJoYXJib3ItcmVnaXN0cnkiLCJleHAiOjE3MzU0NTk0NDcsIm5iZiI6MTczNTQ1NzY0NywiaWF0IjoxNzM1NDU3NjQ3LCJqdGkiOiJER0pqZEN1b3ZiQVY0TjhOIiwiYWNjZXNzIjpbeyJ0eXBlIjoicmVwb3NpdG9yeSIsIm5hbWUiOiJsaWJyYXJ5L2J1c3lib3hpbWFnZTUiLCJhY3Rpb25zIjpbImRlbGV0ZSIsInB1bGwiLCJwdXNoIiwiKiJdfV19.LdcOQQ3X19qFYly7y3H02BD3_SkaNS3-UnzVmu9yQqPuf_TGSAu_OjQATON5oC5jYul-2LtNi0f_C_ZdOf8vFjWa6rLyufe_7XHxvZrOu87L5T94mzXzA07I6YR6hAgS8YnSaa6yTzJ_lu5DYIOJ08mbq4c3A526BYAH6rbzEzNylgb9g4yioUNjMhW1wrrN9PNQd3zzRB_H4F4yMyr3E7S4u8Tsez6C0a6Ihon2MQm13ow57fubksPZPMRHHpQEoMXASijQg7Y0Qsw_cqDP5aJ0Q0vfziiuw8DfuoFjFhspuBl2cxwhgLL_VGwu7QIDytapv8S0R_bNbXaxM-8mioMfJZ6rpeQOJncjnjKTwy9fotPhzxEPAbgraicI0sfedeVtA-NAmAWojuQm_izNL8MUn4Zv6pkJ-IyCu_0db7v8i5-8xfrOagi-LIWdo7-gpq6c4ncAiDpcugOI7aBAEzWv0qt9uINP41vkDfiRe2TJk_QpYGc5MAgEmX5lalFwiVuB0__Ljwsf1UYTnpy8M5amnOPNm6MAdkJzIFSSY3wLMKtepg7cV8CQe3LT4HeaI9kMyzJfgajFiGCQE_bSded9EoZd4h2p_Z8EtU52s_xgHZcx0kAVq0Ld1w2Pe4XZIMQ6xFN5FBeridcKtpy82T-hHPB9mcds-kcBtrpUazA");
        req.set("x-harbor-csrf-token","RcCxkHRTfsVVBA6G9q6MUNXE1rvgnMQfzKhk5CEwSHUpSlyLAhSGbijBROzI8TDBwlfWx4sBA1zpS7SafrCXzg==");
        req.set(beast::http::field::user_agent, "Boost.Beast/248");

        // 发送请求
        beast::http::write(stream, req);
        std::cout << "HTTP request sent." << std::endl;

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        // 关闭连接
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);

        // 检查响应状态
        if (res.result() != beast::http::status::ok) {
            std::cerr << "HTTP request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
            return;
        }

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        // 输出响应体到文件
        // std::ofstream ofs(output_file);
        std::ofstream ofs(output_file, std::ios::binary); // 打开文件为二进制模式
        if (!ofs) {
            std::cerr << "Failed to open file for writing: " << output_file << std::endl;
            return;
        }

        ofs << res.body(); // 将响应体写入文件
        ofs.close();
        std::cout << "Manifest saved to: " << output_file << std::endl;

    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}

int main(){
    try{
        std::string cookie = "_gorilla_csrf=MTczNTQ1NjQ0OHxJbUV3ZWt0MGNIRldOVEY2TUROMVNTOWxRM0JNTnpCUFJVVkdkblZLUWtaNVdrSnRWM050U3pFemMwMDlJZ289fFvtJxOEdkC7vMGjHJs3eYO4cyYakQ3PKvM_bM7wUBuR;sid=bba13b18e047dc060d237fcaac9f743a;";
        // login("admin","Harbor12345");
        pullManifest("192.168.182.128","80");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}