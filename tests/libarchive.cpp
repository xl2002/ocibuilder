#include <iostream>
#include <string>
#include <vector>
//#include "include\libarchive\archive.h"
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>
//#include "include\libarchive\archive_entry.h"

// 添加文件到压缩包
void add_file_to_archive(struct archive *a, const std::string &filename) {
    struct archive_entry *entry;
    struct stat st;

    // 获取文件信息
    if (stat(filename.c_str(), &st) ){
        std::cerr << "Error: Cannot stat file " << filename << std::endl;
        return;
    }

    // 创建 archive entry
    entry = archive_entry_new();
    archive_entry_set_pathname(entry, filename.c_str());
    archive_entry_set_size(entry, st.st_size);
    archive_entry_set_filetype(entry, AE_IFREG);
    archive_entry_set_perm(entry, 0644); // 设置文件权限

    // 写入 entry 头信息
    archive_write_header(a, entry);

    // 写入文件内容
    char buff[8192];
    FILE *file = fopen(filename.c_str(), "rb");
    if (file) {
        size_t len = fread(buff, 1, sizeof(buff), file);
        while (len > 0) {
            archive_write_data(a, buff, len);
            len = fread(buff, 1, sizeof(buff), file);
        }
        fclose(file);
    }

    archive_entry_free(entry);
}

int main() {
    std::cout<<"Starting"<<std::endl;
    // 1. 定义要压缩的文件列表
    std::vector<std::string> files_to_archive = {
        "182f7fe5f39439844fa081ef621851a9d5f4c6b6d5d1be9a008ece843c62eeb6"
    };

    // 2. 创建 archive 对象
    struct archive *a = archive_write_new();
    
    // 3. 设置压缩格式（这里用 .tar.gz）
    //archive_write_add_filter_gzip(a); // 使用 gzip 压缩
    archive_write_set_format_ustar(a); // 标准 tar 格式

    // 4. 打开输出文件
    const char *out_filename = "output.tar";//"\\tests\\output.tar";
    // if (archive_write_open_filename(a, out_filename)) {
    //     std::cerr << "Error: Cannot open output file " << out_filename << std::endl;
    //     return 1;
    // }
    int ret = archive_write_open_filename(a, out_filename);
    if (ret != ARCHIVE_OK) {
        std::cerr << "Error: " << archive_error_string(a) << std::endl;
        std::cerr << "Failed path: " << out_filename << std::endl;
        return 1;
    }

    // 5. 添加文件到压缩包
    for (const auto &file : files_to_archive) {
        add_file_to_archive(a, file);
    }

    // 6. 关闭 archive
    archive_write_close(a);
    archive_write_free(a);



    std::cout << "Successfully created " << out_filename << std::endl;
    return 0;
}