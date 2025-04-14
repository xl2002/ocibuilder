#include "image/buildah/digester.h"
#include "image/digest/algorithm.h"
#include "image/buildah/tar.h"
/**
 * @brief 获取摘要器的内容类型
 * @return std::string 内容类型字符串
 */
std::string simpleDigester::ContentType(){
    return contentType;
}
/**
 * @brief 将data下入到Hash_256中的data
 * 
 * @param data 
 */

void simpleDigester::write(const std::string& data){
    if (this->hasher) {
        // this->hasher->Process(data);
        //计算hash值
        this->hasher->data=this->hasher->Hash_num(std::vector<uint8_t>(data.begin(), data.end()));
    }
}
/**
 * @brief 关闭摘要器并释放资源
 */
void simpleDigester::close(){
    return;
}
/**
 * @brief 获取当前计算的摘要值
 * @return std::shared_ptr<::Digest> 摘要对象指针
 */
std::shared_ptr<::Digest> simpleDigester::Digest(){
    return this->digester->Digest();
}
/**
 * @brief 新建SimpleDigester
 * 
 * @param contentType 
 * @return std::shared_ptr<digester_interface> 
 */
std::shared_ptr<digester_interface> newSimpleDigester(string contentType){
    // 创建一个新的 simpleDigester 实例
    auto finalDigester=Canonical_sha256.Digester();
    auto digester = std::make_shared<simpleDigester>();
    // 设置内容类型
    digester->contentType = contentType;
    // 初始化 hasher，这里假设使用 SHA256
    digester->hasher =finalDigester->GetHash();
    digester->digester = finalDigester;
    return digester;
}
/**
 * @brief 关闭当前打开的摘要器
 */
void CompositeDigester::closeOpenDigester(){
    if(closer!=nullptr){
        closer->close();
        closer=nullptr;
    }
}

// 返回合成的摘要
std::pair<std::string, std::shared_ptr<Digest>> CompositeDigester::Digest() {
    closeOpenDigester();
    size_t num = digesters.size();
    switch (num) {
        case 0:
            return {"", nullptr};
        case 1: {
            auto digester = digesters[0];
            return {digester->ContentType(), digester->Digest()};
        }
        default: {
            std::ostringstream content;
            for (size_t i = 0; i < digesters.size(); ++i) {
                if (i > 0) {
                    content << ",";
                }
                auto digester = digesters[i];
                std::string contentType = digester->ContentType();
                if (!contentType.empty()) {
                    contentType += ":";
                }
                content << contentType << digester->Digest()->Encoded();
            }
            auto compositeDigest = Canonical_sha256.FromString(content.str());
            // 假设 digest.Canonical_interface.FromString 函数的功能
            return {"multi", compositeDigest};
        }
    }
}
/**
 * @brief 重置复合摘要器状态
 * 关闭所有打开的摘要器并清空摘要器列表
 */
void  CompositeDigester::Restart(){
    closeOpenDigester();
    digesters.clear();
}
/**
 * @brief 新建digester
 * 
 * @param contentType 
 */
void  CompositeDigester::Start(std::string contentType){
    this->closeOpenDigester();
    if(contentType.empty()){
        auto newDigester = newSimpleDigester(contentType);
        digesters.push_back(newDigester);
    }else if(contentType=="file"|| contentType=="dir"){
        std::shared_ptr<digester_interface> digester;
        // std::tie(digester, std::ignore)=newTarDigester(contentType,"",fs::path{""});
        // closer = digester;
        digesters.push_back(digester);
    }
}
/**
 * @brief 返回目前实例的hasher
 * 
 * @return std::shared_ptr<digester_interface> 
 */
std::shared_ptr<digester_interface>  CompositeDigester::Hash(){
    // 检查 digesters 向量是否非空
    if (!digesters.empty()) {
        // 返回向量中的最后一个 digester
        return digesters.back();
    }
    // 如果 digesters 为空，则返回 nullptr
    return nullptr;
}
