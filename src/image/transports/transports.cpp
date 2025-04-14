#include "image/transports/transports.h"
// auto kt=std::make_shared<knownTransports>();
/**
 * @brief 根据名称获取已注册的传输协议对象
 * 
 * @param name 传输协议名称
 * @return std::shared_ptr<ImageTransport_interface> 对应的传输协议对象指针，未找到返回nullptr
 */
std::shared_ptr<ImageTransport_interface> Get(std::string name) {
    return kt->get(name);
}
/**
 * @brief 删除指定名称的传输协议
 * 
 * @param name 要删除的传输协议名称
 */
void Delete(std::string name) {
    kt->Remove(name);
}

/**
 * @brief 注册新的传输协议
 * 
 * @param t 要注册的传输协议对象指针
 * @throws myerror 如果协议名称已存在
 */
void Register(std::shared_ptr<ImageTransport_interface> t) {
    kt->Add(t);
}
/**
 * @brief 获取指定名称的传输协议对象(内部实现)
 * 
 * @param name 传输协议名称
 * @return std::shared_ptr<ImageTransport_interface> 对应的传输协议对象指针，未找到返回nullptr
 */
std::shared_ptr<ImageTransport_interface> knownTransports::get(std::string name){
    std::lock_guard<std::mutex> lock(mu);
    if(transports.find(name)==transports.end()){
        return nullptr;
    }
    return transports[name];
}
/**
 * @brief 移除指定名称的传输协议(内部实现)
 * 
 * @param k 要移除的传输协议名称
 */
void knownTransports::Remove(std::string k){
    std::lock_guard<std::mutex> lock(mu);
    transports.erase(k);
}
/**
 * @brief 添加新的传输协议(内部实现)
 * 
 * @param t 要添加的传输协议对象指针
 * @throws myerror 如果协议名称已存在
 */
void knownTransports::Add(std::shared_ptr<ImageTransport_interface> t){
    std::lock_guard<std::mutex> lock(mu);
    auto name=t->Name();
    if(transports.find(name)!=transports.end()){
        throw myerror("Duplicate image transport name "+name);
    }
    transports[name]=t;
}
/**
 * @brief 生成完整的镜像名称字符串
 * 
 * @param ref 镜像引用对象
 * @return std::string 格式为"transport:reference"的完整镜像名称
 */
std::string ImageName(std::shared_ptr<ImageReference_interface> ref) {
    return ref->Transport()->Name()+":"+ref->StringWithinTransport();
}
