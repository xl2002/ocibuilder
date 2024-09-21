#include "storage/images.h"
//recomputeDigests 函数的实现
bool bigDataNameIsManifest(const std::string& name) {
    // 这里 ImageDigestManifestBigDataNamePrefix 是定义为大数据名称的前缀
    const std::string ImageDigestManifestBigDataNamePrefix = "manifest"; 
    return name.find(ImageDigestManifestBigDataNamePrefix) == 0;
}
void storage::Image::recomputeDigests() {
    try {
        // 存储唯一的有效摘要
        std::vector<Digest> validDigests;
        std::set<Digest> uniqueDigests;

        // 处理固定的摘要
        if (!digest->digest.empty()) {
            try {
                digest->Validate();
            }catch (const myerror& e) {
                throw myerror("Validating image digest failed.");
            }
            uniqueDigests.insert(*digest);
            validDigests.push_back(*digest);
        }

        // 处理大数据摘要
        for ( auto& bs : BigDataDigests) {
            auto name=bs.first;
            auto bigDataDigest=bs.second;
            if (!bigDataNameIsManifest(name)) {
                continue;
            }
            try{
                bigDataDigest.Validate();
            }catch (const myerror& e) {
                throw myerror("Validating digest failed for big data item.");
            }
            // 去重
            if (uniqueDigests.find(bigDataDigest) == uniqueDigests.end()) {
                uniqueDigests.insert(bigDataDigest);
                validDigests.push_back(bigDataDigest);
            }
        }

        // 如果固定摘要为空且有效摘要不为空，设置第一个有效摘要
        if (digest->digest.empty() && !validDigests.empty()) {
            digest = std::shared_ptr<Digest>(&validDigests[0]);
        }

        // 设置计算出的摘要列表
        Digests = validDigests;
    } catch (const myerror& e) {
        // 捕获并重新抛出 myerror 类型的异常
        throw;
    }
}