#include "network/network.h"
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
    return std::make_shared<URL>();
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