#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include <iostream>
#include <vector>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;

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
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 连接到服务器
        auto const results = resolver.resolve("localhost", "5000");
        stream.connect(results);
        std::string file_path = "C:\\Users\\xiongkang\\Desktop\\oci_registry\\blobs\\sha256\\b3268e691068e94bdff71d38b6b895fd5cb8a584183ac11ce8cac56d325de43f";
        // 打开文件
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("无法打开文件！");
        }

        file.seekg(0, std::ios::end);
        std::size_t total_size = file.tellg();
        file.seekg(0, std::ios::beg);

        const std::size_t chunk_size = 512 * 1024; // 每次传输 512 KB
        send_chunked_patch_request(stream, "localhost:5000", "/v2/busybox/blobs/uploads/", file, chunk_size, total_size);

        std::cout << "上传完成！" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
    }

    return 0;
}
