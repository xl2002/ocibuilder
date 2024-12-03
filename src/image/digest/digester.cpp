#include "image/digest/digester.h"

std::shared_ptr<Hash_256> digester:: Hash(){
    
    return this->hash;
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

    return std::vector<uint8_t>();
}