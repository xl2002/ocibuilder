#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <regex>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

std::tuple<std::string, std::string, std::string> initUpload(const std::string& host, const std::string& port, const std::string& target,const std::string& cookie) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        beast::http::request<beast::http::empty_body> req(beast::http::verb::post, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,cookie);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("x-harbor-csrf-token","luiwMoxecFyl/YJwpJYfulHictn5usZzwn3A+gP9Jnb6Y2DFdgF37ZZVwDElccBIYV284MLLVpmAcvkwtpWN6w==");
        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::accepted) {
            std::cout<<res.body()<<"\n";
            throw std::runtime_error("Failed to initiate upload");
            
        }
        
        std::cout<<res.body()<<"\n";

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
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

        std::string harbor_csrf_token;
        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                harbor_csrf_token = token;
                break;
            }
        }
        return {uid_parm,state_param,harbor_csrf_token};

        throw std::runtime_error("Invalid Location URL format.");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return {"", "",""};
    }
}

std::pair<std::string,std::string> uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
                            const std::string& file_path, std::size_t start, std::size_t end, std::size_t total_size,const std::string& cookie,const std::string& token) {
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

        std::string target = "/v2/library/busybox/blobs/uploads/" + uid + "?_state=" + state;


        beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, "application/octet-stream");
        // req.set(beast::http::field::content_encoding, "gzip"); 
        req.set(beast::http::field::content_range, "bytes=" + std::to_string(start) + "-" + std::to_string(end - 1) + "/" + std::to_string(total_size));
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("x-harbor-csrf-token",token);
        req.set(beast::http::field::cookie,cookie);

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);
        std::cout << "success patch!! " << "\n";

        beast::flat_buffer buffer_r;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer_r, res);
        std::cout << "success res!!! " << "\n";
        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::accepted) {
            std::cout<<res.body();
            throw std::runtime_error("Failed to upload blob chunk");
        }

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        // std::string new_state = std::string(res["Docker-Upload-State"].begin(), res["Docker-Upload-State"].end());
        std::string location = res[beast::http::field::location].to_string();
        std::string new_state;
        auto pos1 = location.find("_state=");
        if (pos1 != std::string::npos) {
            new_state = location.substr(pos1 + 7);
            std::cout << "State parameter: " << new_state << "\n";
        }


        std::string harbor_csrf_token;
        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                harbor_csrf_token = token;
                break;
            }
        }

        return {new_state,harbor_csrf_token};
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return {state,token};
    }
}


std::string uploadConfig(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
                            const std::string& file_path, std::size_t start, std::size_t end,const std::string& cookie) {
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

        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/busybox/blobs/uploads/" + uid + "?_state=" + state;

        beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, "application/vnd.docker.container.image.v1+json");
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set(beast::http::field::cookie,cookie);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);
        std::cout << "success put!! " << "\n";

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

void uploadManifest(const std::string& host, const std::string& port, const std::string& file_path, 
                        std::size_t start, std::size_t end,const std::string& cookie) {
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

        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/library/busybox/manifests/latest";

        beast::http::request<beast::http::buffer_body> req(beast::http::verb::put, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, "application/vnd.docker.distribution.manifest.v2+json");
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(beast::http::field::cookie,cookie);
        req.set("x-harbor-csrf-token","LPHQ1DzGxZRp/GOntzyjJTpM4jNZJKTj3LXPax4alD9AegAjxpnCJVpUIeY223zXCvMsCmJVNAmeuvahq3I/og==");


        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);
        std::cout << "success put!! " << "\n";

        beast::flat_buffer buffer_r;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer_r, res);
        std::cout << "success res!!! " << "\n";
        stream.socket().shutdown(tcp::socket::shutdown_both);

        if (res.result() != beast::http::status::ok && res.result() != beast::http::status::created) {
            for (auto const& field : res) {
                std::cout << field.name_string() << ": " << field.value() << "\n";
            }
            throw std::runtime_error("Failed to upload manifest");
        }

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

void finalizeUpload(const std::string& host, const std::string& port, const std::string& uid, const std::string& digest, const std::string& state,const std::string& cookie,const std::string& token) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/library/busybox/blobs/uploads/" + uid + "?_state=" + state + "&digest=" + digest;

        beast::http::request<beast::http::empty_body> req(beast::http::verb::put, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,cookie);
        req.set("x-harbor-csrf-token",token);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        if (res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to finalize upload");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

int main() {
    try {
        std::string host = "192.168.182.128";
        std::string port = "80";
        std::string cookie = "_gorilla_csrf=MTczNDg3Mjg0NXxJbUpKZGxFNUwzQm1RamRGZW5GRlNrSm5aV1ptT0dwREwzcHFhemRqV2tSeFVXYzROWGx5Vm05eE5UQTlJZ289fLAperNqJGnO0kWux4zpdo3rNHhL5sQYRXjw3csgriq7;sid=238f99d6c796386af39196d230d82790;";

        std::string target = "/v2/library/busybox/blobs/uploads/";
        std::string file_path = "E:/work/code/OCI/ocibuilder/busybox/blobs/sha256/f0/f0a3fe58120bd28b27186346f7a7320fdf48665a91d6b76e9d20ffdcac39d15b/data";
        // std::string file_path ="E:/work/code/OCI/ocibuilder/oci_registry/oci_registry/blobs/sha256/9f38a27f64274b1f8d3f7812dbe4dd3909a338a5b3d1adc589cc24e8f0d3ae0c";

        // std::ifstream file(file_path, std::ios::binary | std::ios::ate);
        // if (!file) {
        //     throw std::runtime_error("Failed to open file: " + file_path);
        // }
        // std::size_t total_size = file.tellg();
        // file.close();

        std::size_t chunk_size = 8192 * 1024;

        // // POST 请求初始化上传
        // std::string uid;
        // std::string state;
        // std::string token;
        // std::tie(uid,state,token) = initUpload(host, port, target,cookie);

        // if (uid.empty() || state.empty()) {
        //     throw std::runtime_error("Failed to initialize upload.");
        // }

        // // PATCH 请求上传分片
        // std::size_t start = 0;
        // while (start < total_size) {
        //     std::size_t end = std::min(start + chunk_size, total_size);
        //     std::pair<std::string, std::string> re = uploadBlobChunk(host, port, uid, state, file_path, start, end, total_size,cookie,token);
        //     state=re.first;
        //     token=re.second;
        //     start = end;
        //     std::cout << "Patch success!!" <<"\n";
        //     std::cout<<"State-->"<<state<<"\n";
        // }
        // // state = uploadBlobChunk(host, port, uid, state, file_path, start, total_size, total_size);
        // // PUT 请求完成上传
        // std::string digest = "sha256:f0a3fe58120bd28b27186346f7a7320fdf48665a91d6b76e9d20ffdcac39d15b";
        // finalizeUpload(host, port, uid, digest, state,cookie,token);
//config---------------------------------------------------
        // std::string file_path1 = "E:/work/code/OCI/ocibuilder/busybox/blobs/sha256/af/af47096251092caf59498806ab8d58e8173ecf5a182f024ce9d635b5b4a55d66/data";

        // std::ifstream file1(file_path1, std::ios::binary | std::ios::ate);
        // if (!file1) {
        //     throw std::runtime_error("Failed to open file: " + file_path1);
        // }
        // std::size_t total_size1 = file1.tellg();
        // file1.close();


        // // POST 请求初始化上传
        // std::string uid1;
        // std::string state1;
        // std::string token1;
        // std::tie(uid1,state1,token1) = initUpload(host, port, target,cookie);
        // if (uid1.empty() || state1.empty()) {
        //     throw std::runtime_error("Failed to initialize upload.");
        // }

        // // PATCH 请求上传分片
        // std::size_t start1 = 0;
        // while (start1 < total_size1) {
        //     std::size_t end = std::min(start1 + chunk_size, total_size1);
        //     std::pair<std::string, std::string> re1 = uploadBlobChunk(host, port, uid1, state1, file_path1, start1, end, total_size1,cookie,token1);
        //     state1=re1.first;
        //     token1=re1.second;
        //     start1 = end;
        //     std::cout << "Config success!!" <<"\n";
        //     std::cout<<"State-->"<<state1<<"\n";
        // }
        // // state = uploadBlobChunk(host, port, uid, state, file_path, start, total_size, total_size);
        // // PUT 请求完成上传
        // std::string digest1 = "sha256:af47096251092caf59498806ab8d58e8173ecf5a182f024ce9d635b5b4a55d66";
        // finalizeUpload(host, port, uid1, digest1, state1,cookie,token1);

// //manifest-----------------------------
        std::string file_path2 = "E:/work/code/OCI/ocibuilder/busybox/blobs/sha256/93/9307d4acaeed71c4c6ba84494aef0464ffc446a5d91f0a7c196fa8eda1bf0590/data";
        // std::string file_path2 ="E:/work/code/OCI/ocibuilder/oci_registry/oci_registry/blobs/sha256/85742db45ae6e67e64f0ff1797c9e373ab5f3633eb07c006b27870bf4c22cfe8";
        std::ifstream file2(file_path2, std::ios::binary | std::ios::ate);
        if (!file2) {
            throw std::runtime_error("Failed to open file: " + file_path2);
        }
        std::size_t total_size2 = file2.tellg();
        file2.close();

        std::size_t start2 = 0;
        while (start2 < total_size2) {
            std::size_t end = std::min(start2 + chunk_size, total_size2);
            uploadManifest(host, port,file_path2, start2, end, cookie);
            start2 = end;
            std::cout << "Manifest success!!" <<"\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
