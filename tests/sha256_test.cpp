// #include <boost/filesystem.hpp>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iomanip>
// namespace fs = boost::filesystem;

// 计算文件的SHA256哈希值
// std::string calculate_sha256(const std::string& filepath) {
//     std::ifstream file(filepath, std::ios::binary);
//     if (!file.is_open()) {
//         throw std::runtime_error("无法打开文件: " + filepath);
//     }

//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);

//     char buffer[8192];
//     while (file.read(buffer, sizeof(buffer))) {
//         SHA256_Update(&sha256, buffer, file.gcount());
//     }
//     // 处理最后剩余的数据
//     if (file.gcount() > 0) {
//         SHA256_Update(&sha256, buffer, file.gcount());
//     }

//     unsigned char hash[SHA256_DIGEST_LENGTH];
//     SHA256_Final(hash, &sha256);

//     // 转换为十六进制字符串
//     std::ostringstream oss;
//     for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
//         oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
//     }
//     return oss.str();
// }
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
int main() {
    try {
        std::string folder = "your_folder";  // 待打包文件夹
        std::string tar_path = "output.tar";
        std::string gzip_path = "output.tar.gz";
        std::string path ="./tests/87ff76f62d367950186bde563642e39208c0e2b4afc833b4b3b01b8fef60ae9e";
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("无法打开文件: " + path);
        }

        // 获取文件大小
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // 读取文件内容到vector
        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            throw std::runtime_error("读取文件失败: " + path);
        }
        auto ret=calculate_sha256(buffer);
        // 转换为十六进制字符串
        std::ostringstream oss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << (int)ret[i];
        }
        // return 
        std::cout<<"文件夹SHA256: "<<oss.str()<<std::endl;
        // 创建tar包
        // create_tar(folder, tar_path);
        // std::cout << "Tar包SHA256: " << calculate_sha256(tar_path) << std::endl;

        // // 压缩为gzip
        // compress_gzip(tar_path, gzip_path);
        // std::cout << "Gzip包SHA256: " << calculate_sha256(gzip_path) << std::endl;

    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
    }

    return 0;
}