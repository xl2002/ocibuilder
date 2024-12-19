#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

std::pair<std::string, std::string> initUpload(const std::string& host, const std::string& port, const std::string& target) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        beast::http::request<beast::http::empty_body> req(beast::http::verb::post, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::accepted) {
            throw std::runtime_error("Failed to initiate upload");
        }

        std::string location = res[beast::http::field::location].to_string();
        std::string uid_parm;
        std::string state_param;

        auto pos = location.find("/uploads/");
        if (pos != std::string::npos) {
            // 截取 "/uploads/" 后面的部分
            uid_parm = location.substr(pos + 9); // 截取 "uploads/" 后面的部分
            // 检查 URL 后面的参数（_state=）并去掉它
            auto state_pos = uid_parm.find("?");
            if (state_pos != std::string::npos) {
                uid_parm = uid_parm.substr(0, state_pos); // 去掉 "?_state=" 后面的部分
            }
        }

        auto pos1 = location.find("_state=");
        if (pos1 != std::string::npos) {
            state_param = location.substr(pos1 + 7);
            std::cout << "State parameter: " << state_param << "\n";
        }
        return {uid_parm,state_param};

        throw std::runtime_error("Invalid Location URL format.");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return {"", ""};
    }
}

std::string uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
                            const std::string& file_path, std::size_t start, std::size_t end, std::size_t total_size) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        std::cout << "Uploading chunk: Start = " << start << ", End = " << end << "\n";

        std::size_t chunk_size = end - start;
        std::vector<char> data(chunk_size);

        file.seekg(start, std::ios::beg);
        file.read(data.data(), chunk_size);
        std::size_t bytes_read = file.gcount();

        // boost::asio::streambuf stream_buffer;
        // std::ostream os(&stream_buffer);
        // std::vector<char> buffer(chunk_size);  // 使用一个临时缓冲区来存储每次读取的数据
        // file.read(buffer.data(), chunk_size);  // 从文件中读取 chunk_size 大小的块

        // std::size_t bytes_read = file.gcount();  // 获取实际读取的字节数
        // if (bytes_read != chunk_size && file.eof() == false) {
        //     throw std::runtime_error("Failed to read the requested chunk from file.");
        // }
        // os.write(buffer.data(), bytes_read);

        if (file.gcount() != static_cast<std::streamsize>(chunk_size)) {
            throw std::runtime_error("Failed to read the requested chunk from file.");
        }


        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/busybox/blobs/uploads/" + uid + "?_state=" + state;


        beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, "application/octet-stream");
        // req.set(beast::http::field::content_encoding, "gzip"); 
        req.set(beast::http::field::content_range, "bytes=" + std::to_string(start) + "-" + std::to_string(end - 1) + "/" + std::to_string(total_size));
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        // req.body() = beast::http::buffer_body::value_type(asio::buffer(data.data(), data.size()));
        // req.body() = boost::asio::buffer(data.data(), data.size());
        req.prepare_payload();
        // 创建一个 boost::asio::mutable_buffer 来包装读取的数据

        // req.body().data = const_cast<void*>(stream_buffer.data().data());
        // req.body().size = chunk_size;
        // req.prepare_payload();

        beast::http::write(stream, req);
        std::cout << "success patch!! " << "\n";

        beast::flat_buffer buffer_r;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer_r, res);
        std::cout << "success res!!! " << "\n";
        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::accepted) {
            throw std::runtime_error("Failed to upload blob chunk");
        }

        // std::string new_state = std::string(res["Docker-Upload-State"].begin(), res["Docker-Upload-State"].end());
        std::string location = res[beast::http::field::location].to_string();
        std::string new_state;
        auto pos1 = location.find("_state=");
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

void finalizeUpload(const std::string& host, const std::string& port, const std::string& uid, const std::string& digest, const std::string& state) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/busybox/blobs/uploads/" + uid + "?_state=" + state + "&digest=" + digest;

        beast::http::request<beast::http::empty_body> req(beast::http::verb::put, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

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

int main() {
    try {
        std::string host = "localhost";
        std::string port = "5000";
        std::string target = "/v2/busybox/blobs/uploads/";
        std::string file_path = "E:\\BaiduSyncdisk\\OCI\\oci_images\\oci_registry\\blobs\\sha256\\d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24";

        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        std::size_t total_size = file.tellg();
        file.close();

        std::size_t chunk_size = 4096 * 1024;

        // POST 请求初始化上传
        std::pair<std::string, std::string> result = initUpload(host, port, target);
        std::string uid = result.first;
        std::string state = result.second;
        if (uid.empty() || state.empty()) {
            throw std::runtime_error("Failed to initialize upload.");
        }

        // PATCH 请求上传分片
        std::size_t start = 0;
        while (start < total_size) {
            std::size_t end = std::min(start + chunk_size, total_size);
            state = uploadBlobChunk(host, port, uid, state, file_path, start, end, total_size);
            start = end;
            std::cout << "Patch success!!" <<"\n";
            std::cout<<"State-->"<<state<<"\n";
        }
        // state = uploadBlobChunk(host, port, uid, state, file_path, start, total_size, total_size);
        // PUT 请求完成上传
        std::string digest = "sha256:d9a1c0e63b46b78ce21a60cef1fa8240a4b6fbe58c2b6a70c52e70f248375f24";
        finalizeUpload(host, port, uid, digest, state);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
