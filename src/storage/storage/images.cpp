#include "storage/storage/images.h"
#include "utils/logger/ProcessSafeLogger.h"
// #include "image/buildah/image.h"
//recomputeDigests 函数的实现
/**
 * @brief 检查大数据名称是否是manifest类型
 * @param name 要检查的大数据名称
 * @return bool 如果名称以"manifest"开头返回true，否则返回false
 */
bool bigDataNameIsManifest(const std::string& name) {
    // 这里 ImageDigestManifestBigDataNamePrefix 是定义为大数据名称的前缀
    const std::string ImageDigestManifestBigDataNamePrefix = "manifest"; 
    return name.find(ImageDigestManifestBigDataNamePrefix) == 0;
}
/**
 * @brief 重新计算图像的摘要信息
 * @details 该函数会：
 *          1. 验证并收集固定摘要
 *          2. 验证并收集大数据摘要
 *          3. 去重处理所有摘要
 *          4. 如果固定摘要为空，则设置第一个有效摘要
 * @throws myerror 当摘要验证失败时抛出
 */
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
                logger->log_error("Validating image digest failed: %s"+std::string(e.what()));
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
                logger->log_error("Validating digest failed for big data item: %s"+std::string(e.what()));
                throw myerror("Validating digest failed for big data item.");
            }
            // 去重
            if (uniqueDigests.find(bigDataDigest) == uniqueDigests.end()) {
                uniqueDigests.insert(bigDataDigest);
                validDigests.push_back(bigDataDigest);
            }
        }

        // 如果固定摘要为空且有效摘要不为空，设置第一个有效摘要
        if (digest->digest.empty() && !Digests.empty()) {
            digest = std::make_shared<Digest>(Digests[0]);
        }

        // 设置计算出的摘要列表
        // Digests = validDigests;
    } catch (const myerror& e) {
        // 捕获并重新抛出 myerror 类型的异常
        throw;
    }
}
