#include "image/digest/digester.h"

/**
 * @brief 获取当前哈希值
 * @return std::shared_ptr<Hash_256> 返回哈希实例
 * @details 返回digester当前持有的哈希对象
 */
std::shared_ptr<Hash_256> digester:: GetHash(){
    
    return this->hash;
}
/**
 * @brief 获取当前算法
 * @return std::shared_ptr<Algorithm_sha256> 返回算法实例
 * @details 返回digester当前使用的算法对象
 */
std::shared_ptr<Algorithm_sha256> digester::GetAlgorithm() const {
    return this->alg;
}
/**
 * @brief 设置算法
 * @param algorithm 要设置的算法实例
 * @details 更新digester使用的算法对象
 */
void digester::SetAlgorithm(const std::shared_ptr<Algorithm_sha256>& algorithm) {
    alg = algorithm;
}
/**
 * @brief 生成摘要
 * @return std::shared_ptr<Digest> 返回摘要实例
 * @details 使用当前算法和哈希值创建新的摘要对象
 */
std::shared_ptr<::Digest> digester::Digest(){
    return NewDigest(this->alg,this->hash);
}
/**
 * @brief 计算数据的SHA256哈希值
 * @param data 输入的需要计算的数据
 * @return std::vector<uint8_t> 返回32字节的SHA256哈希值
 * @details 使用OpenSSL SHA256算法计算输入数据的哈希值。
 *          如果输入为空，返回初始哈希值(全零)。
 *          实现参考OpenSSL SHA256接口和sha256_test.cpp中的示例。
 */
std::vector<uint8_t> Hash_256::Hash_num(const std::vector<uint8_t>& data){
    // const BYTE* byteData = reinterpret_cast<const BYTE*>(data.data());
    // BYTE buf[SHA256_BLOCK_SIZE];
    // SHA256_CTX ctx;
    // sha256_init(&ctx);
    // sha256_update(&ctx, byteData, strlen(reinterpret_cast<const char*>(byteData)));
    // sha256_final(&ctx, buf);
    // std::vector<uint8_t> hashResult(buf, buf + sizeof(buf));
    // 初始化 SHA256_CTX
    // 创建 SHA256 上下文
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // 更新 SHA256 上下文
    SHA256_Update(&sha256, data.data(), data.size());

    // 获取最终哈希值
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    
    std::stringstream ss;
    for (auto byte : hash) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
    }
    std::string ret=ss.str();
    // 将结果存储到 vector<uint8_t> 中
    return std::vector<uint8_t>(hash, hash + SHA256_DIGEST_LENGTH);
}
