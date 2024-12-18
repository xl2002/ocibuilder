#include "image/digest/digester.h"

std::shared_ptr<Hash_256> digester:: GetHash(){
    
    return this->hash;
}
std::shared_ptr<Algorithm_sha256> digester::GetAlgorithm() const {
    return this->alg;
}
// Setter for alg
void digester::SetAlgorithm(const std::shared_ptr<Algorithm_sha256>& algorithm) {
    alg = algorithm;
}
std::shared_ptr<::Digest> digester::Digest(){
    return NewDigest(this->alg,this->hash);
}
/**
 * @brief 根据data计算哈希值
 *  参考src\image\digest\sha256_test.cpp中的做法，
 *  注意变量类型的转换，函数接受空输入，空输入直接返回初始哈希值
 * 
 * @param data 输入的需要计算的数据
 * @return std::vector<uint8_t> 返回数据的sha256值
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
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // 更新哈希计算
    SHA256_Update(&sha256, data.data(), data.size());

    // 生成哈希值
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256_Final(hash.data(), &sha256);

    // std::stringstream ss;
    // for (auto byte : hash) {
    //     ss << std::setw(2) << std::setfill('0') << std::hex << (int)byte;
    // }
    // std::string ret=ss.str();
    return hash;
}