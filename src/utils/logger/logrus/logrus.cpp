/** @file logrus.cpp 
 * @brief Logrus日志级别实现 
 */

#include "utils/logger/logrus/logrus.h"

/** @brief 将日志级别转换为字符串表示
 * @return 返回日志级别的字符串表示，转换失败返回"unknown"
 */
std::string Level::String() {
    try
    {
        auto b=MarshalText();
        return std::string(b.begin(),b.end());
    }
    catch(const myerror& e)
    {
        return "unknown";
    }
}

/** @brief 将日志级别序列化为文本
 * @return 返回包含日志级别名称的字节向量
 * @throw myerror 当日志级别无效时抛出异常
 */
std::vector<uint8_t> Level::MarshalText(){
    if(lvl==level::TraceLevel){
        return std::vector<uint8_t>({'t', 'r', 'a', 'c', 'e'});
    }
    if(lvl==level::DebugLevel){
        return std::vector<uint8_t>({'d', 'e', 'b', 'u', 'g'});
    }
    if(lvl==level::InfoLevel){
        return std::vector<uint8_t>({'i', 'n', 'f', 'o'});
    }
    if(lvl==level::WarnLevel){
        return std::vector<uint8_t>({'w', 'a', 'r', 'n'});
    }
    if(lvl==level::ErrorLevel){
        return std::vector<uint8_t>({'e', 'r', 'r', 'o', 'r'});
    }
    if(lvl==level::FatalLevel){
        return std::vector<uint8_t>({'f', 'a', 't', 'a', 'l'});
    }
    if(lvl==level::PanicLevel){
        return std::vector<uint8_t>({'p', 'a', 'n', 'i', 'c'});
    }
    throw myerror("not a valid logrus level "+std::to_string((int)lvl));
}
/** @brief 解析字符串为日志级别
 * @param lvl 要解析的日志级别字符串
 * @return 返回对应的Level对象
 * @throw myerror 当字符串不是有效的日志级别时抛出异常
 */
Level ParseLevel(std::string lvl){
    std::transform(lvl.begin(), lvl.end(), lvl.begin(), ::tolower);
    if(lvl=="trace"){
        return Level(level::TraceLevel);
    }
    else if(lvl=="debug"){
        return Level(level::DebugLevel);
    }
    else if(lvl=="info"){
        return Level(level::InfoLevel);
    }
    else if(lvl=="warn"){
        return Level(level::WarnLevel);
    }
    else if(lvl=="error"){
        return Level(level::ErrorLevel);
    }
    else if(lvl=="fatal"){
        return Level(level::FatalLevel);
    }
    else if(lvl=="panic"){
        return Level(level::PanicLevel);
    }else{
        throw myerror("not a valid logrus Level: "+lvl);
    }
}
/** @brief 从字节向量反序列化日志级别
 * @param text 包含日志级别名称的字节向量
 * @throw myerror 当反序列化失败时抛出异常
 */
void Level:: UnmarshalText(std::vector<uint8_t> text){
    try
    {
        auto l=ParseLevel(std::string(text.begin(),text.end()));
        lvl=l.lvl;
    }
    catch(const myerror& e)
    {
        throw;
    }
}
