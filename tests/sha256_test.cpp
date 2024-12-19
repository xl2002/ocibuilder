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
// namespace fs = boost::filesystem;
namespace io = boost::iostreams;
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
// // }
// std::vector<uint8_t> calculate_sha256(const std::vector<uint8_t>& input_data) {
//     // 初始化 SHA256_CTX
//     SHA256_CTX sha256;
//     SHA256_Init(&sha256);

//     // 更新哈希计算
//     SHA256_Update(&sha256, input_data.data(), input_data.size());

//     // 生成哈希值
//     std::vector<uint8_t> hash(SHA256_DIGEST_LENGTH);
//     SHA256_Final(hash.data(), &sha256);

//     return hash;
// }
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
    return oss.str();
}

// 将文件夹打包为tar文件
void create_tar(const std::string& folder_path, const std::string& tar_path) {
    struct archive* a = archive_write_new();
    struct archive_entry* entry;
    if (!a) {
        throw std::runtime_error("无法创建archive对象");
    }
    archive_write_set_format_pax_restricted(a); // 设置tar格式
    if (archive_write_open_filename(a, tar_path.c_str()) != ARCHIVE_OK) {
        archive_write_free(a);
        throw std::runtime_error("无法打开tar文件: " + tar_path);
    }

    fs::recursive_directory_iterator dir_iter(folder_path), end_iter;
    for (; dir_iter != end_iter; ++dir_iter) {
        const fs::path& file_path = dir_iter->path();
        std::string relative_path = fs::relative(file_path, folder_path).string();

        entry = archive_entry_new();
        archive_entry_set_pathname(entry, relative_path.c_str());
        archive_entry_set_size(entry, fs::file_size(file_path));
        archive_entry_set_filetype(entry, S_IFREG);
        archive_entry_set_perm(entry, 0644);

        archive_write_header(a, entry);

        std::ifstream file(file_path.string(), std::ios::binary);
        char buffer[8192];
        while (file.read(buffer, sizeof(buffer))) {
            archive_write_data(a, buffer, file.gcount());
        }
        if (file.gcount() > 0) {
            archive_write_data(a, buffer, file.gcount());
        }
        archive_entry_free(entry);
    }

    archive_write_close(a);
    archive_write_free(a);
}

void compress_gzip(const std::string& tar_path, const std::string& gzip_path) {
    struct archive* a = archive_write_new();
    struct archive* input = archive_read_new();
    if (!a || !input) {
        throw std::runtime_error("无法创建archive对象");
    }

    archive_write_add_filter_gzip(a);
    if (archive_write_open_filename(a, gzip_path.c_str()) != ARCHIVE_OK) {
        archive_write_free(a);
        archive_read_free(input);
        throw std::runtime_error("无法打开gzip文件: " + gzip_path);
    }

    archive_read_support_format_tar(input);
    if (archive_read_open_filename(input, tar_path.c_str(), 10240) != ARCHIVE_OK) {
        archive_write_free(a);
        archive_read_free(input);
        throw std::runtime_error("无法打开tar文件: " + tar_path);
    }

    struct archive_entry* entry;
    while (archive_read_next_header(input, &entry) == ARCHIVE_OK) {
        archive_write_header(a, entry);

        const void* buffer;
        size_t size;
        la_int64_t offset;
        while (archive_read_data_block(input, &buffer, &size, &offset) == ARCHIVE_OK) {
            archive_write_data(a, buffer, size);
        }
    }

    archive_write_close(a);
    archive_read_close(input);
    archive_write_free(a);
    archive_read_free(input);
}

void compress_gzip2(const std::string& tar_path, const std::string& gzip_path) {
    // 打开tar文件
    std::ifstream tar_file(tar_path, std::ios::binary);
    if (!tar_file.is_open()) {
        throw std::runtime_error("无法打开tar文件: " + tar_path);
    }

    // 打开gzip压缩流
    std::ofstream gzip_file(gzip_path, std::ios::binary);
    if (!gzip_file.is_open()) {
        throw std::runtime_error("无法打开gzip文件: " + gzip_path);
    }

    // 设置gzip过滤器
    io::filtering_streambuf<io::output> out;
    out.push(io::gzip_compressor());  // 添加gzip压缩
    out.push(gzip_file);  // 将gzip压缩流输出到目标文件

    // 将tar文件内容逐块写入压缩流
    char buffer[8192];
    while (tar_file.read(buffer, sizeof(buffer)) || tar_file.gcount() > 0) {
        std::ostream(&out) << std::string(buffer, tar_file.gcount());
    }

    // 关闭文件
    tar_file.close();
    gzip_file.close();
}
int main() {
    try {
        std::string folder = "./tests/a";  // 待打包文件夹
        std::string tar_path = "./tests/output";
        // std::string gzip_path = "./tests/output";
        std::string gzip_path2="./tests/output2";
        // std::string path ="./tests/87ff76f62d367950186bde563642e39208c0e2b4afc833b4b3b01b8fef60ae9e";
        // std::ifstream file(path, std::ios::binary);
        // if (!file.is_open()) {
        //     throw std::runtime_error("无法打开文件: " + path);
        // }

        // 获取文件大小
        // file.seekg(0, std::ios::end);
        // std::streamsize size = file.tellg();
        // file.seekg(0, std::ios::beg);

        // // 读取文件内容到vector
        // std::vector<uint8_t> buffer(size);
        // if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        //     throw std::runtime_error("读取文件失败: " + path);
        // }
        // auto ret=calculate_sha256(buffer);
        // // 转换为十六进制字符串
        // std::ostringstream oss;
        // for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        //     oss << std::hex << std::setw(2) << std::setfill('0') << (int)ret[i];
        // }
        // // return 
        // std::cout<<"文件夹SHA256: "<<oss.str()<<std::endl;
        // 创建tar包
        // create_tar(folder, tar_path);
        // std::cout << "Tar包SHA256: " << calculate_sha256(tar_path) << std::endl;

        // // 压缩为gzip
        // compress_gzip(tar_path, gzip_path);
        // std::cout << "Gzip包SHA256: " << calculate_sha256(gzip_path) << std::endl;
            // 创建tar包
        create_tar(folder, tar_path);
        std::cout << "Tar包SHA256: " << calculate_sha256(tar_path) << std::endl;

        // 压缩为gzip
        // compress_gzip(tar_path, gzip_path);
        // std::cout << "Gzip包SHA256: " << calculate_sha256(gzip_path) << std::endl;

        compress_gzip2(tar_path, gzip_path2);
        std::cout << "Gzip包SHA256: " << calculate_sha256(gzip_path2) << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "错误: " << ex.what() << std::endl;
    }

    return 0;
}