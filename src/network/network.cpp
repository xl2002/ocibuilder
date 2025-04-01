#include "network/network.h"
#include "image/digest/digest.h"
#include "cmd/login/login.h"
#include <zlib/zlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
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
/**
 * @brief 根据路径解析请求的URL
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

        std::size_t colonPos = path.find(':');
        std::size_t slashPos = path.find('/');
        std::size_t lastSlash = path.find_last_of('/');

        // if (colonPos == std::string::npos || slashPos == std::string::npos || colonPos > slashPos) {
        //     throw std::invalid_argument("Invalid path format");
        // }

        std::string host;
        if(colonPos == std::string::npos){
            host = path.substr(0,slashPos);
        }else{
            host = path.substr(0, colonPos);
        }
        
        //默认端口号是80
        std::string portStr="80";
        if(slashPos - colonPos - 1 > 0){
            portStr = path.substr(colonPos + 1, slashPos - colonPos - 1);
        }

        std::size_t secondSlashAfterHost = path.find('/', slashPos + 1);

        std::string projectName;
        std::string imageName;
        std::string version;
        std::size_t colon = path.find(':', lastSlash);

        projectName = path.substr(slashPos + 1, secondSlashAfterHost - slashPos - 1);

        if (colon == std::string::npos) {
            // 没有 ':'，说明没有 tag，直接返回 '/' 后的部分
            imageName=path.substr(lastSlash + 1);
            version="latest";
        } else {
            // 有 ':'，返回 ':' 之前的部分
            imageName=path.substr(lastSlash + 1, colon - lastSlash - 1);
            version=path.substr(colon+1);
        }

        url->host=host;
        url->port=portStr;
        url->imageName=imageName;
        url->version=version;
        url->projectName=projectName;

        return url;
    }

    
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
 * @return 
 */
bool ifBlobExists(const std::string& host,const std::string& port,const std::string& imageName,const std::string& shaId,const std::string& projectName){
    try {
        // 配置参数
        const std::string target="/v2/"+projectName+"/"+imageName+"/blobs/sha256:"+shaId;

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 解析并连接到主机
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 构造 HTTP HEAD 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::head, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        if (!loginAuth.bearerToken.empty()) {
            setAuthorization(req, loginAuth.bearerToken);
        } else {
            setAuthorization(req, userinfo.username, userinfo.password);
        }

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();


        // 根据状态码判断 blob 是否存在
        if (res.result() == beast::http::status::ok) {
            return true;
        }
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Blob Exist Error: " << e.what() << "\n";
    }
    return false;
}

/**
 * @brief 该manifest是否在服务器中存在
 * @param host 
 * @param port 
 * @param imageName 
 * @param shaId 
 * @return 
 */
bool ifManifestExists(const std::string& host,const std::string& port,const std::string& imageName, const std::string version,const std::string& projectName){
    try {
        // 配置参数
        const std::string target="/v2/"+projectName+"/"+imageName+"/manifests/"+version;//+'/'+shaId;

        // IO 上下文
        asio::io_context ioc;

        // 解析器和流
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 解析并连接到主机
        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 构造 HTTP HEAD 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::head, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        if (!loginAuth.bearerToken.empty()) {
            setAuthorization(req, loginAuth.bearerToken);
        } else {
            setAuthorization(req, userinfo.username, userinfo.password);
        }

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();


        // 根据状态码判断 blob 是否存在
        if (res.result() == beast::http::status::ok) {
            return true;
        }
        stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
    } catch (const std::exception& e) {
        std::cerr << "Manifest Exist Error: " << e.what() << "\n";
    }
    return false;
}


/**
 * @brief 上传数据前初始化请求，以获得uid和state
 * @param host 
 * @param port 
 * @param target 
 * @return 
 */
std::pair<std::string, std::string> initUpload(const std::string& host, const std::string& port,const std::string& imageName,const std::string& projectName) {
    try {

        const std::string target="/v2/"+projectName+"/"+imageName+"/blobs/uploads/";

        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        beast::http::request<beast::http::empty_body> req(beast::http::verb::post, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        req.set(http::field::accept_encoding, "gzip");
        if (!loginAuth.bearerToken.empty()) {
            setAuthorization(req, loginAuth.bearerToken);
        } else {
            setAuthorization(req, userinfo.username, userinfo.password);
        }

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();

        if (res.result() != beast::http::status::accepted) {
            throw std::runtime_error("Failed to initiate upload");
        }

        std::string location = res[beast::http::field::location].to_string();
        std::string ipAddr = host + ":" + port;
        auto pos_start = location.find(ipAddr);
        if (pos_start != std::string::npos) {
            loginAuth.location = location.substr(pos_start + ipAddr.length());
        }
        std::string uid_param;
        std::string state_param;

        // 获取uid和state
        for (auto const& field : res) {
            if (field.name_string() == "Docker-Upload-Uuid") {
                uid_param = field.value().to_string();
            }
        }
        // 这里很让人无语，harbor是用_state=来标识state的，而acore是state_=
        
        auto pos = location.find("?_state=");
        if (pos != std::string::npos) {
            state_param = location.substr(pos + 8);
        } else {
            pos = location.find("?state_=");
            if (pos != std::string::npos) {
                state_param = location.substr(pos + 8);
            }
            else
                throw std::runtime_error("Failed to get state");
        }

        stream.socket().shutdown(tcp::socket::shutdown_both);

        return {uid_param,state_param};

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
 * @return 
 */
std::pair<std::string, std::string> uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
                            const std::string& file_path, std::size_t start, std::size_t end, std::size_t total_size,const std::string& imageName,const std::string& projectName) {
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

        // buildah这里是upload，不是uploads
        std::string target;
        // if (loginAuth.bearerToken.empty()) {
        //     target = "/" + projectName + "/"+imageName + "/blobs/upload/" + uid + "?state_=" + state;
        // } else {
        //     target = "/v2/" + projectName + "/"+imageName + "/blobs/uploads/" + uid + "?_state=" + state;
        // }
        target = loginAuth.location;
        beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
        req.set(beast::http::field::host, host + ":" + port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
            // req.set(http::field::cookie, loginAuth.cookie);
        } else {
            std::string credentials = userinfo.username + ":" + userinfo.password;
            std::string encoded_credentials = base64_encode(credentials);
            req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(beast::http::field::content_type, "application/octet-stream");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();

        if (res.result() != beast::http::status::accepted) {
            throw std::runtime_error("Failed to upload blob chunk");
        }

        std::string location = res[beast::http::field::location].to_string();
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
        // 使用acore方式
        if (loginAuth.bearerToken.empty()) {
            auto pos = location.find("state_=");
            if (pos != std::string::npos) {
                new_state = location.substr(pos + 7);
            }
        } 
        // 使用harbor方式
        else {
            auto pos = location.find("_state=");
            if (pos != std::string::npos) {
                new_state = location.substr(pos + 7);
            }
        }
        stream.socket().shutdown(tcp::socket::shutdown_both);
        return {new_uid, new_state};
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
    auto pos = new_path.find_first_of("/");
    if (pos != std::string::npos) {
        new_path = new_path.substr(pos + 1);  // 去掉开头的斜杠
    }
    return new_path;
}

/**
 * @brief 上传manifest
 * @param host 
 * @param port 
 * @param file_path 
 * @param start 
 * @param end 
 * @param imageName 
 * @param version 
 */
void uploadManifest(const std::string& host, const std::string& port, const std::string& file_path, std::size_t start, std::size_t end, 
                                            const std::string& imageName, const std::string version, const std::string& ManifestType,const std::string& projectName, 
                                            bool v1, const std::string& store_basic_path) {
    try {
        // std::ifstream file(file_path, std::ios::binary);
        // if (!file) {
        //     throw std::runtime_error("Failed to open file: " + file_path);
        // }
        std::size_t chunk_size = end - start;
        std::vector<char> data(chunk_size);
        std::size_t bytes_read;
        // 如果采用v1版本，则需要创建一个临时的manifest文件
        if (v1 == true) {
            std::string new_path = write_manifest_new(file_path);
            // 去掉最后一个目录后加上新的路径
            std::string::size_type pos = store_basic_path.find_last_of("\\");
            if (pos != std::string::npos) {
                new_path = store_basic_path.substr(0, pos + 1) + new_path;
            }
            std::ifstream file(new_path, std::ios::binary);
            file.seekg(start, std::ios::beg);
            file.read(data.data(), chunk_size);
            bytes_read = file.gcount();
        } else {
            std::ifstream file(file_path, std::ios::binary);
            file.seekg(start, std::ios::beg);
            file.read(data.data(), chunk_size);
            bytes_read = file.gcount();
        }


        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        std::string target = "/v2/"+projectName+"/"+imageName+"/manifests/"+version;

        beast::http::request<beast::http::buffer_body> req(beast::http::verb::put, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        if (!loginAuth.bearerToken.empty()) {
            req.set(http::field::authorization, "Bearer " + loginAuth.bearerToken);
        } else {
            std::string credentials = userinfo.username + ":" + userinfo.password;
            std::string encoded_credentials = base64_encode(credentials);
            req.set(http::field::authorization, "Basic " + encoded_credentials);
        }
        req.set(beast::http::field::content_type, ManifestType);
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::accept_encoding, "gzip");
        req.set(http::field::connection, "close");

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::dynamic_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();

        if (res.result() != beast::http::status::ok && res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to upload manifest");
        }
        stream.socket().shutdown(tcp::socket::shutdown_both);
        
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
 */
void finalizeUpload(const std::string& host, const std::string& port, const std::string& uid, const std::string& shaId, const std::string& state,const std::string& imageName,const std::string& projectName) {
    try {
        asio::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(host, port);
        stream.connect(results);

        // 这里按照biuldah也是upload，不是uploads
        std::string target;
        // if (loginAuth.bearerToken.empty()) {
        //     target = "/" +projectName + "/"+ imageName + "/blobs/upload/" + uid + "?state_=" + state + "&digest=sha256:" + shaId;        
        // } else {
        //     target = "/v2/" +projectName + "/"+ imageName + "/blobs/uploads/" + uid + "?_state=" + state + "&digest=sha256:" + shaId;
        // }
        target = loginAuth.location + "&digest=sha256:" + shaId;
        // std::string target = "/v2/" +projectName + "/"+ imageName + "/blobs/uploads/" + uid + "?_state=" + state + "&digest=sha256:" + shaId;
        beast::http::request<beast::http::empty_body> req(beast::http::verb::put, target, 11);
        req.set(http::field::host, host+":"+port);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(beast::http::field::content_type, "application/octet-stream");
        req.set("Docker-Distribution-Api-Version", "registry/2.0");
        req.set(http::field::connection, "close");
        req.set(http::field::accept_encoding, "gzip");
        if (!loginAuth.bearerToken.empty()) {
            setAuthorization(req, loginAuth.bearerToken);
            // req.set(http::field::cookie, loginAuth.cookie);
        } else {
            setAuthorization(req, userinfo.username, userinfo.password);
        }

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response_parser<beast::http::empty_body> parser;
        parser.body_limit(std::numeric_limits<std::uint64_t>::max());
        parser.skip(true);
        beast::http::read(stream, buffer, parser);
        auto res = parser.get();
        if (res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to finalize upload");
        }
        stream.socket().shutdown(tcp::socket::shutdown_both);
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
    const std::string& projectName, const std::string& imageName) {
    try {
        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
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
            bool receive_cookie = false;
            std::string sid;
            // 获取cookie的长度
            for (auto const& field : res) {
                if (field.name_string() == "Set-Cookie") {
                    receive_cookie = true;

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
            // 关闭连接
            stream.socket().shutdown(asio::ip::tcp::socket::shutdown_both);
            // 如果接受到cookie，说明是harbor方式，需要继续发送请求
            if (receive_cookie) {
                // 第二次发送请求
                std::string btoken = loginGetToken(host, port, user, passwd, projectName, imageName);
                return btoken;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return "";
}

bool login(const std::string& host, const std::string& port, const std::string& user, const std::string& passwd) {
    try {

        // 创建 I/O 上下文
        asio::io_context ioc;

        // 解析主机名和端口
        std::string target = "/v2/";  // 根据需要修改路径
        int version = 11;  // HTTP 版本，1.1
        
        // 创建 TCP 解析器
        asio::ip::tcp::resolver resolver(ioc);
        // 问题在这里
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
        if (!loginAuth.bearerToken.empty()) {
            setAuthorization(req, loginAuth.bearerToken);
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


std::tuple<std::string,size_t> pullManifestAndBlob(const std::string& host, const std::string& port,const::string& projectName,const::string& imageName,const std::string version,const std::string& os,const std::string& arch){
    // std::string target = "/v2/library/busyboximage5/blobs/sha256:f28efabc598d38f0b7cea1641bd20b097b8c5aaf090035d7167370608d86fb67"; // API v2 路径
    std::string target= "/v2/"+projectName+"/"+imageName+"/manifests/"+version;
    std::string output_folder = "oci_images/oci_registry/blobs/sha256/"; // 保存的文件夹
    std::string output_file_tmp =  "oci_images/tmp/manifest"; // 完整的文件路径

    try {
        // 确保文件夹存在
        if (!fs::exists(output_folder)) {
            fs::create_directories(output_folder);
            std::cout << "Created directory: " << output_folder << std::endl;
        }
        if(!fs::exists("oci_images/tmp/")){
            fs::create_directories("oci_images/tmp/");
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
        req.set(beast::http::field::accept, 
            "application/vnd.docker.distribution.manifest.v2+json, "\
            "application/vnd.oci.image.manifest.v1+json, "\
            "application/vnd.docker.distribution.manifest.v1+prettyjws, "\
            "application/vnd.docker.distribution.manifest.v1+json, "\
            "application/vnd.docker.distribution.manifest.list.v2+json, "\
            "application/vnd.oci.image.index.v1+json");
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
            return {};
        }

        // for (auto const& field : res) {
        //     std::cout << field.name_string() << ": " << field.value() << "\n";
        // }


        //分析manifest
        auto manifest=unmarshal<::Manifest>(res.body());

        //pull config  分析os和arch是否符合，不符合直接退出
        if(!pullConfig(host,port,projectName,imageName,std::string(manifest.Config.Digests.digest),os,arch)) return{};


        // 输出响应体到文件
        // std::ofstream ofs(output_file);
        std::ofstream ofs(output_file_tmp, std::ios::binary); // 打开文件为二进制模式
        if (!ofs) {
            std::cerr << "Failed to open file for writing: " << output_file_tmp << std::endl;
            return {};
        }


        ofs << res.body(); // 将响应体写入文件
        ofs.close();
        // std::cout << "Manifest tmp saved to: " << output_file_tmp << std::endl;

        std::string ManifestSha;
        //写manifest, 改为直接写, 不进行格式上的转换
        // if (v1 == true) {
        //     std::string new_path = write_manifest_new(output_file_tmp);
        //     // std::string new_path = write_v1_manifest(output_file_tmp);
        //     boost::filesystem::path source = new_path;
        //     std::cout << new_path << std::endl;
        //     ManifestSha = source.filename().string();
        //     boost::filesystem::path target = output_folder + source.filename().string();

        //     if (boost::filesystem::exists(source)) {
        //         boost::filesystem::copy_file(source, target, fs::copy_option::overwrite_if_exists);
        //     } else {
        //         throw std::runtime_error("Failed to copy file: " + source.string());
        //     }
        // } else {
        //     // 计算manifest的hash
        ManifestSha = Fromfile(output_file_tmp)->Encoded();
        MediaTypeImageManifest = manifest.MediaType;
        std::string output_file = output_folder + ManifestSha;

        if(std::remove(output_file_tmp.c_str())==0){
            // std::cout << "tmp manifest deleted successfully: " << output_file_tmp << std::endl;
        }else{
            // std::cerr << "Failed to delete file: " << output_file_tmp << std::endl;
            throw std::runtime_error("Failed to delete temp file: " + output_file_tmp);
        }
        std::ofstream ofs1(output_file, std::ios::binary);
        if (!ofs1) {
            throw std::runtime_error("Failed to open file for writing: " + output_file);
            // std::cerr << "Failed to open file for writing: " << output_file << std::endl;
            return {};
        }

        ofs1 << res.body(); 
        ofs1.close();
        // }

        size_t manifestLen;

        if (res.find(beast::http::field::content_length) != res.end()) {
            std::string content_length = res[beast::http::field::content_length].to_string();
            manifestLen = std::stoul(content_length);
            // std::cout << "Content-Length: " << content_length << std::endl;
        } else {
            std::cout << "No Content-Length field found in response." << std::endl;
        }

        //依次pull数据层
        std::vector<Descriptor> Layers=manifest.Layers;
        for(std::size_t i=0;i<Layers.size();i++){
            Descriptor de=Layers[i];
            pullBlob(host,port,projectName,imageName,std::string(de.Digests.digest));
        }

        return std::make_tuple("sha256:"+ManifestSha,manifestLen);

    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Pull Manifest Exception: " << e.what() << std::endl;
    }
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


std::vector<std::string> getTagList(const std::string& host, const std::string& port,const::string& projectName,const::string& imagetName){
    try{
        std::string target="/v2/"+projectName+"/"+imagetName+"/tags/list";

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
        req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::authorization,"Bearer "+loginAuth.bearerToken);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // 发送请求
        beast::http::write(stream, req);

        // 接收响应
        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);
        stream.socket().shutdown(tcp::socket::shutdown_both);
        
        // for (auto const& field : res) {
        //     std::cout << field.name_string() << ": " << field.value() << "\n";
        // }

        if (res.result() != beast::http::status::ok) {
            std::cerr << "GetTagsList request failed with status: " << res.result_int() << " " << res.reason() << std::endl;
            return {};
        }

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken=token;
                break;
            }
        }

        std::vector<std::string> tags;
        // 解析响应体中的 JSON 数据
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
    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
        return {};
    }

}