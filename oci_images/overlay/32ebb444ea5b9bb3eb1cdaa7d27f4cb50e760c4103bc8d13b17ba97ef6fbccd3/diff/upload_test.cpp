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
        req.set("Docker-Distribution-API-Version", "registry/2.0");
        req.prepare_payload();

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

        // Create a buffer to hold the chunk
        boost::asio::streambuf buffer;
        std::ostream buffer_stream(&buffer);

        // Read the chunk from the file
        // const std::size_t chunk_size = end - start;
        // char* chunk_data = new char[chunk_size];
        // file.read(chunk_data, chunk_size);
        // buffer_stream.write(chunk_data, file.gcount());
        // delete[] chunk_data;

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
        req.set(beast::http::field::content_encoding, "gzip"); 
        req.set(beast::http::field::content_range, "bytes=" + std::to_string(start) + "-" + std::to_string(end - 1) + "/" + std::to_string(total_size));
        req.set(beast::http::field::content_length, std::to_string(chunk_size));
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-API-Version", "registry/2.0");
        req.body().more=true;
        // req.body().data = data.data();
        // req.body().data = const_cast<char*>(data.data());
        // req.body().data = std::move(file);
        // // req.body().data = const_cast<void*>(buffer.data().data());
        // boost::asio::mutable_buffer buffer = boost::asio::buffer(data.data(), data.size());
        // beast::flat_buffer b;
        // b.data.data();
        // b.data = dynamic_cast<void*>(buffer.data());
        req.body().data=data.data();
        req.body().size = buffer.size();
    //         auto& body_buffer = req.body().data(); // 获取动态缓冲区的引用
    // auto buffers = body_buffer.prepare(data.size()); // 准备缓冲区
    // std::memcpy(boost::asio::buffer_cast<void*>(buffers), data.data(), data.size());
    // body_buffer.commit(data.size()); // 提交写入的数据
        // req.body().size = bytes_read; 
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
namespace http = beast::http;
void upload_file(const std::string& file_path, const std::string& host, const std::string& target,const std::string& state) {
    const std::size_t chunk_size = 512 * 1024; // 每次上传 512 KB 数据
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + file_path);
    }

    // 获取文件大小
    std::size_t file_size = file.tellg();
    file.seekg(0);

    // 创建 I/O 上下文和连接
    asio::io_context ioc;
    asio::ip::tcp::resolver resolver(ioc);
    beast::tcp_stream stream(ioc);

    // 解析主机名并建立连接
    auto const results = resolver.resolve(host, "5000");
    stream.connect(results);
    auto state1=state;
    std::string target1;
    std::size_t offset = 0;
    while (offset < file_size) {
        target1 = target + "?_state=" + state1;
        // 计算本次传输的大小
        std::size_t bytes_to_read = std::min(chunk_size, file_size - offset);

        // 读取数据
        std::vector<char> buffer(bytes_to_read);
        file.read(buffer.data(), bytes_to_read);

        // 创建 PATCH 请求
        http::request<http::buffer_body> req(http::verb::patch, target1, 11);
        req.set(http::field::host, host);
        req.set(http::field::content_type, "application/octet-stream");
        req.set(http::field::content_range,
                "bytes " + std::to_string(offset) + "-" + std::to_string(offset + bytes_to_read - 1) + "/" +
                std::to_string(file_size));
        req.set(http::field::content_length, std::to_string(bytes_to_read));
        req.set("Docker-Distribution-API-Version", "registry/2.0");

        req.body().data = buffer.data();
        req.body().size = bytes_to_read;
        req.body().more = offset + bytes_to_read < file_size; // 判断是否还有更多数据

        // 发送请求
        beast::flat_buffer buffer3;
        http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer1;
        http::response<http::string_body> res;
        http::read(stream, buffer1, res);

        // 打印响应状态
        if (res.result() != http::status::ok && res.result() != http::status::created) {
            throw std::runtime_error("上传失败，状态码: " + std::to_string(res.result_int()));
        }

        // 打印返回信息
        std::cout << "Chunk uploaded: " << offset << "-" << offset + bytes_to_read - 1 << std::endl;

        // 更新偏移量
        offset += bytes_to_read;
        std::string location = res[beast::http::field::location].to_string();
        auto pos1 = location.find("_state=");
        if (pos1 != std::string::npos) {
            state1 = location.substr(pos1 + 7);
            std::cout << "State parameter: " << state1 << "\n";
        }
    }

    // 关闭流
    stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
}
void send_chunked_patch_request(beast::tcp_stream& stream, const std::string& host, const std::string& target, std::ifstream& file, std::size_t chunk_size, std::size_t total_size) {
    std::vector<char> buffer(chunk_size);
    std::size_t offset = 0;

    while (offset < total_size) {
        // 计算当前块的大小
        std::size_t bytes_to_read = std::min(chunk_size, total_size - offset);
        buffer.resize(bytes_to_read);
        file.read(buffer.data(), bytes_to_read);

        // 设置 PATCH 请求
        http::request<http::buffer_body> req(http::verb::patch, target, 11);
        req.set(http::field::host, host);
        req.set(http::field::content_type, "application/octet-stream");
        req.set(http::field::content_range,
                "bytes " + std::to_string(offset) + "-" + std::to_string(offset + bytes_to_read - 1) + "/" + std::to_string(total_size));
        req.set(http::field::content_length, std::to_string(bytes_to_read));
        req.set("Docker-Distribution-API-Version", "registry/2.0");

        // 设置缓冲区
        req.body().data = const_cast<char*>(buffer.data());
        req.body().size = bytes_to_read;

        // 如果还有数据要发送，则设置 more = true，否则为 false
        req.body().more = (offset + bytes_to_read < total_size);

        // 发送请求
        http::write(stream, req);

        // 更新偏移量
        offset += bytes_to_read;
    }
}
int main() {
    try {
        std::string host = "localhost";
        std::string port = "5000";
        std::string target = "/v2/busybox/blobs/uploads/";
        std::string file_path = "C:\\Users\\xiongkang\\Desktop\\oci_registry\\blobs\\sha256\\b3268e691068e94bdff71d38b6b895fd5cb8a584183ac11ce8cac56d325de43f";

        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        std::size_t total_size = file.tellg();
        file.close();

        std::size_t chunk_size = 512 * 1024;

        // POST 请求初始化上传
        std::pair<std::string, std::string> result = initUpload("localhost", port, target);
        
        std::string uid = result.first;
        std::string state = result.second;
        if (uid.empty() || state.empty()) {
            throw std::runtime_error("Failed to initialize upload.");
        }
        std::string target1 = "/v2/busybox/blobs/uploads/" + uid+"?_state="+state;
        try {
            asio::io_context ioc;
            asio::ip::tcp::resolver resolver(ioc);
            beast::tcp_stream stream(ioc);

            // 连接到服务器
            auto const results = resolver.resolve("localhost", "5000");
            stream.connect(results);
            // std::string file_path = "C:\\Users\\xiongkang\\Desktop\\oci_registry\\blobs\\sha256\\b3268e691068e94bdff71d38b6b895fd5cb8a584183ac11ce8cac56d325de43f";
            // 打开文件
            std::ifstream file(file_path, std::ios::binary);
            if (!file) {
                throw std::runtime_error("无法打开文件！");
            }

            file.seekg(0, std::ios::end);
            std::size_t total_size = file.tellg();
            file.seekg(0, std::ios::beg);

            const std::size_t chunk_size = 512 * 1024; // 每次传输 512 KB
            send_chunked_patch_request(stream, "localhost:5000", target1, file, chunk_size, total_size);

            std::cout << "上传完成！" << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "错误: " << ex.what() << std::endl;
        }
        // upload_file(file_path, host, target1,state);
        // PATCH 请求上传分片
        // std::size_t start = 0;
        // while (start < total_size) {
        //     std::size_t end = std::min(start + chunk_size, total_size);
        //     state = uploadBlobChunk(host, port, uid, state, file_path, start, end, total_size);
        //     start = end;
        //     std::cout << "Patch success!!" <<"\n";
        //     std::cout<<"State-->"<<state<<"\n";
        // }
        // state = uploadBlobChunk(host, port, uid, state, file_path, start, total_size, total_size);
        // PUT 请求完成上传
        std::string digest = "sha256:b3268e691068e94bdff71d38b6b895fd5cb8a584183ac11ce8cac56d325de43f";
        finalizeUpload(host, port, uid, digest, state);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
