#include "network/network.h"
#include "image/digest/digest.h"

/**
 * @brief 创建新的请求
 * 
 * @param method 请求的方式（HEAD，GET，POST等）
 * @param path 请求的网络路径或者target
 * @param headers 网络头的信息
 * @param body 所要传输的数据
 * @return beast::http::request<beast::http::string_body> 
 */
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
            std::cout<<token<<"\n";
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


        std::string token=getToken(host,port,projectName,imageName);

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
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::authorization,"Bearer "+token);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // 发送请求
        beast::http::write(stream, req);

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
            }
        }
        // 根据状态码判断 blob 是否存在
        if (res.result() == beast::http::status::ok) {
            return true;
        } else {
            return false;
        } 
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
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
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        for (auto const& field : res) {
            std::cout << field.name_string() << ": " << field.value() << "\n";
        }

        std::cout<<res.body()<<"\n";
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


        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }



        return {uid_parm,state_param};

        throw std::runtime_error("Invalid Location URL format.");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
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
std::string uploadBlobChunk(const std::string& host, const std::string& port, const std::string& uid, const std::string& state,
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

        std::string target = "/v2/" + projectName + "/"+imageName + "/blobs/uploads/" + uid + "?_state=" + state;
        beast::http::request<beast::http::buffer_body> req(beast::http::verb::patch, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, "application/octet-stream");
        req.set(beast::http::field::content_range, "bytes=" + std::to_string(start) + "-" + std::to_string(end - 1) + "/" + std::to_string(total_size));
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);

        beast::flat_buffer buffer_r;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer_r, res);
        stream.socket().shutdown(tcp::socket::shutdown_both);


        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }
        if (res.result() != beast::http::status::accepted) {
            throw std::runtime_error("Failed to upload blob chunk");
        }

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
                                            const std::string& imageName, const std::string version, const std::string& ManifestType,const std::string& projectName) {
    try {
        std::ifstream file(file_path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + file_path);
        }
        // std::cout << "Uploading chunk: Start = " << start << ", End = " << end << "\n";

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

        std::string target = "/v2/"+projectName+"/"+imageName+"/manifests/"+version;

        beast::http::request<beast::http::buffer_body> req(beast::http::verb::put, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::content_type, ManifestType);
        req.set(beast::http::field::content_length, std::to_string(bytes_read));
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        req.body().data = data.data();
        req.body().size = bytes_read; 
        req.body().more=false;
        req.prepare_payload();

        beast::http::write(stream, req);

        beast::flat_buffer buffer_r;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer_r, res);
        stream.socket().shutdown(tcp::socket::shutdown_both);

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }
        if (res.result() != beast::http::status::ok && res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to upload manifest");
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
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

        std::string target = "/v2/" +projectName + "/"+ imageName + "/blobs/uploads/" + uid + "?_state=" + state + "&digest=sha256:" + shaId;
        beast::http::request<beast::http::empty_body> req(beast::http::verb::put, target, 11);
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
        req.set(beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        beast::http::write(stream, req);

        beast::flat_buffer buffer;
        beast::http::response<beast::http::string_body> res;
        beast::http::read(stream, buffer, res);

        stream.socket().shutdown(tcp::socket::shutdown_both);

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }
        if (res.result() != beast::http::status::created) {
            throw std::runtime_error("Failed to finalize upload");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}

bool isCorrect(std::string sha256,std::string filepath){
    auto digest = Fromfile(filepath);
    return digest->Encoded() == sha256;
}



void login(const std::string& user, const std::string& passwd,const std::string& host,const std::string& port) {

    // 设置认证服务器的URL和请求头
    // std::string host = "192.168.182.128";
    // std::string port = "80";
    std::string target = "/c/login";  
    const int version = 11; // HTTP 1.1
    std::string body = "principal=" + user + "&password=" + passwd;
    std::string authFile = "oci_images/auth.json";


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
    // 发送请求
    beast::http::write(stream, req);

    // 接收响应
    beast::flat_buffer buffer;
    beast::http::response<beast::http::string_body> res;
    beast::http::read(stream, buffer, res);

    for (auto const& field : res) {
        std::cout << field.name_string() << ": " << field.value() << "\n";
    }
    std::cout<<res.body()<<"\n";
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
                            loginAuth.cookie="_gorilla_csrf="+csrf_token+"; sid="+sid+";";

                            // boost::json::object jsonData;
                            // jsonData["cookie"] = "_gorilla_csrf="+csrf_token+"; sid="+sid+";";

                            // std::ofstream ofs(authFile);
                            // if (ofs) {
                            //     ofs << json::serialize(jsonData); // 写入 JSON 文件
                            //     ofs.close(); // 显式关闭文件
                            //     std::cout << "Cookie saved to JSON file: " << authFile << "\n";
                            // } else {
                            //     std::cerr << "Failed to open file for saving cookie.\n";
                            // }
                        }
                    }  
                }
                if (field.name_string() == "X-Harbor-Csrf-Token") {
                    std::string token = field.value().to_string();
                    loginAuth.harborToken = token;
                }
            }
        }

    }

    // 关闭连接
    stream.close();
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
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set(beast::http::field::authorization,"Bearer "+loginAuth.bearerToken);
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
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

        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }

    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void pullManifestAndBlob(const std::string& host, const std::string& port,const::string& projectName,const::string& imageName,const std::string version){
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

        // IO 服务和解析器
        asio::io_context ioc;
        asio::ip::tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // 设置超时时间
        stream.expires_after(std::chrono::seconds(5)); // 超时 5 秒

        // 解析并连接
        auto const results = resolver.resolve(host, port);
        stream.connect(results);



        std::string token=getToken(host,port,projectName,imageName);

        // 设置 HTTP 请求
        beast::http::request<beast::http::empty_body> req(beast::http::verb::get, target, 11); // 使用 GET 请求
        req.set(beast::http::field::host, host);
        req.set(beast::http::field::accept, "application/vnd.oci.image.manifest.v1+json,application/vnd.docker.distribution.manifest.v2+json");
        req.set(beast::http::field::cookie,loginAuth.cookie);
        req.set(beast::http::field::authorization,"Bearer "+loginAuth.bearerToken);
        req.set("x-harbor-csrf-token",loginAuth.harborToken);
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
        std::ofstream ofs(output_file_tmp, std::ios::binary); // 打开文件为二进制模式
        if (!ofs) {
            std::cerr << "Failed to open file for writing: " << output_file_tmp << std::endl;
            return;
        }

        ofs << res.body(); // 将响应体写入文件
        ofs.close();
        std::cout << "Manifest tmp saved to: " << output_file_tmp << std::endl;

        //计算manifest的hash
        std::string ManifestSha = Fromfile(output_file_tmp)->Encoded();
        std::string output_file = output_folder + ManifestSha;

        if(std::remove(output_file_tmp.c_str())==0){
            std::cout << "tmp manifest deleted successfully: " << output_file_tmp << std::endl;
        }else{
            std::cerr << "Failed to delete file: " << output_file_tmp << std::endl;
        }


        //写manifest
        std::ofstream ofs1(output_file, std::ios::binary);
        if (!ofs1) {
            std::cerr << "Failed to open file for writing: " << output_file << std::endl;
            return;
        }

        ofs1 << res.body(); 
        ofs1.close();
        std::cout << "Manifest tmp saved to: " << output_file << std::endl;



        for (auto const& field : res) {
            if (field.name_string() == "X-Harbor-Csrf-Token") {
                std::string token = field.value().to_string();
                loginAuth.harborToken = token;
                break;
            }
        }

        //分析manifest
        auto manifest=unmarshal<::Manifest>(res.body());

        //pull config
        pullBlob(host,port,projectName,imageName,std::string(manifest.Config.Digests.digest));

        //依次pull数据层
        std::vector<Descriptor> Layers=manifest.Layers;
        for(std::size_t i=0;i<Layers.size();i++){
            Descriptor de=Layers[i];
            pullBlob(host,port,projectName,imageName,std::string(de.Digests.digest));
        }

    } catch (const beast::system_error& se) {
        std::cerr << "System error: " << se.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
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
                    std::cout << "Cookie loaded from JSON: " << cookie << "\n";
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

void saveLoginInfo(const std::string& username, const std::string& password){
    std::string authPath = "oci_images/auth.json";
    json::object credentials;
    credentials["username"] = username;
    credentials["password"] = password;

    std::ofstream ofs(authPath);
    if (ofs) {
        ofs << json::serialize(credentials);
        std::cout << "Credentials saved to auth.json\n";
    } else {
        std::cerr << "Failed to save credentials\n";
    }
}

void loadLoginInfo() {
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
        // 提取字段
        if (credentials.contains("username") && credentials.contains("password")) {
            username = json::value_to<std::string>(credentials["username"]);
            password = json::value_to<std::string>(credentials["password"]);
            userinfo.username=username;
            userinfo.password=password;
        } else {
            std::cerr << "auth.json does not contain required fields\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error reading auth.json: " << e.what() << "\n";
    }
}