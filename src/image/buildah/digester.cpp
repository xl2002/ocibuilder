#include "image/buildah/digester.h"
#include "image/digest/algorithm.h"

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
        return {digester->ContentType(), digester->Digester()};
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
            content << contentType << digester->Digester()->Encoded();
        }
        auto compositeDigest = Canonical_sha256.FromString(content.str());
        // 假设 digest.Canonical_interface.FromString 函数的功能
        return {"multi", compositeDigest};
    }
    }
}