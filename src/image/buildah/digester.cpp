#include "image/buildah/digester.h"
#include "image/digest/algorithm.h"
std::string simpleDigester::ContentType(){
    return contentType;
}
/**
 * @brief 将data下入到Hash_256中的data
 * 
 * @param data 
 */
void simpleDigester::write(const std::string& data){
    
}
void simpleDigester::close(){
    return;
}
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
    return nullptr;
}
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

}
/**
 * @brief 返回目前实例的hasher
 * 
 * @return std::shared_ptr<digester_interface> 
 */
std::shared_ptr<digester_interface>  CompositeDigester::Hash(){
    return nullptr;
}