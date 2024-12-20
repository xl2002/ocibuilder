#include <boost/filesystem.hpp>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
// #include <boost/archive/tar.hpp>
// #include <boost/archive/basic_archive.hpp>
// #include <boost/filesystem.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <zlib/zlib.h>
namespace io = boost::iostreams;

std::vector<uint8_t> calculate_sha256(const std::vector<uint8_t>& input_data) {
    // 初始化 SHA256_CTX
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    // 更新哈希计算
    SHA256_Update(&sha256, input_data.data(), input_data.size());

    // 生成哈希值
    std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
    SHA256_Final(hash.data(), &sha256);

    return hash;
}
namespace fs = boost::filesystem;

// 计算文件的SHA256哈希值
std::string calculate_sha256(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filepath);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }
    // 处理最后剩余的数据
    if (file.gcount() > 0) {
        SHA256_Update(&sha256, buffer, file.gcount());
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &sha256);

    // 转换为十六进制字符串
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    file.close();
    return oss.str();
}
int main() {
    std::string filepath = "tests/44c6d8eb73faf2764e5966327386eb9213471ed0bc21f1ea700bce46c1d31e00";

    std::ifstream file(filepath, std::ios::binary);

    std::ostringstream oss;
    oss<<file.rdbuf();
    file.close();
    std::string str=oss.str();
    auto data = std::vector<uint8_t>(str.begin(),str.end());
    auto hash = calculate_sha256(data);
    std::ostringstream oss1;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        oss1 << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    std::cout<<"use vector sha256: "<<oss1.str()<<std::endl;
    std::cout<<"use filepath sha256: "<<calculate_sha256(filepath)<<std::endl;

}