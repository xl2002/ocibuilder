#include "image/digest/digester.h"

digester::digester(std::shared_ptr<Algorithm_sha256> algorithm, std::shared_ptr<Hash_256> hashData)
    : alg(algorithm), hash(hashData) {}

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
    const BYTE* byteData = reinterpret_cast<const BYTE*>(data.data());
    BYTE buf[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, byteData, strlen(reinterpret_cast<const char*>(byteData)));
    sha256_final(&ctx, buf);
    std::vector<uint8_t> hashResult(buf, buf + sizeof(buf));
    return hashResult;
}