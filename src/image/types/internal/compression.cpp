#include "image/types/internal/types.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <zlib/zlib.h>
// #include <boost/iostreams.hpp>
namespace io = boost::iostreams;

/**
 * @brief 将输入流数据压缩为 gzip 格式并输出到输出流
 * @param inputStream 输入流，提供原始数据
 * @param outputStream 输出流，接收压缩后的数据
 */
// void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
//     try {
//         // std::chrono::duration<double> duration;
//         // std::chrono::high_resolution_clock::time_point start,end;
//         // start=std::chrono::high_resolution_clock::now();
//         // 使用 gzip 压缩过滤器
//         io::filtering_stream<io::output> filterStream;
//         filterStream.push(io::gzip_compressor());
//         filterStream.push(outputStream);

//         // 将输入流数据写入过滤流，完成压缩
//         filterStream << inputStream.rdbuf();
//         filterStream.reset(); // 确保所有数据被刷新到输出流

//         // end=std::chrono::high_resolution_clock::now();
//         // duration=end-start;
//         // std::cout << "Compression time: " << duration.count() << " s" << std::endl;
//         // std::cout << "Compression successful." << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error during compression: " << e.what() << std::endl;
//     }
// }
// void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
//     try {
//         // 设置 gzip 参数，保持稳定输出（符合 Docker 规范）
//         io::gzip_params params;
//         params.level = io::gzip::best_compression; // 可选：最高压缩率
//         // params.level = io::gzip::default_compression; // 可选：默认压缩率
//         params.mtime = 0;                          // 固定时间戳（1970-01-01）
//         params.file_name = "";                     // 不包含文件名
//         // params.os_code = 255;                      // 统一 OS（255 = unknown）

//         io::filtering_stream<io::output> filterStream;
//         filterStream.push(io::gzip_compressor(params));
//         filterStream.push(outputStream);

//         filterStream << inputStream.rdbuf();
//         filterStream.reset(); // 刷新并关闭压缩器，写入结束

//     } catch (const std::exception& e) {
//         std::cerr << "Error during compression: " << e.what() << std::endl;
//     }
// }
//与buildah和docker一致
void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
    try {
        // 设置 gzip 参数，使输出具有指定的时间戳（2288912640）
        io::gzip_params params;
        // params.level = io::gzip::best_compression; // 可选：最高压缩率
        params.level = io::gzip::default_compression;  // 设置默认压缩级别
        // params.level = io::gzip::best_speed;     // 设置快速压缩级别
        params.mtime = 2288912640;                     // 使用buildah默认的时间戳（2288912640）2042-07-14 01:04:00 (UTC)
        params.file_name = "";                         // 清除原始文件名字段，避免差异

        // 使用自定义的gzip压缩器
        io::filtering_stream<io::output> filterStream;
        filterStream.push(io::gzip_compressor(params));
        filterStream.push(outputStream);

        // 将输入流数据写入过滤流，完成压缩
        filterStream << inputStream.rdbuf();
        filterStream.reset(); // 刷新并关闭压缩器，确保写完所有数据

    } catch (const std::exception& e) {
        std::cerr << "Error during compression: " << e.what() << std::endl;
    }
}
//直接利用zlib库压缩
// void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
//     // const int level = Z_BEST_SPEED;          // 对应 level 9
//     const int level = Z_DEFAULT_COMPRESSION;          // 对应 level 9
//     const int windowBits = 15 + 16;                // 15: default deflate, +16: GZIP 格式
//     const int memLevel = 8;                        // buildah 默认
//     const int strategy = Z_DEFAULT_STRATEGY;
//     const uLong mtime = 2288912640UL;              // 2042-07-14 01:04:00 UTC

//     z_stream zs;
//     std::memset(&zs, 0, sizeof(zs));

//     // 初始化 deflate，开启 gzip 模式
//     int ret = deflateInit2(&zs, level, Z_DEFLATED, windowBits, memLevel, strategy);
//     if (ret != Z_OK) throw std::runtime_error("deflateInit2 failed");

//     // 设置 gzip 头时间戳
//     gz_header header;
//     std::memset(&header, 0, sizeof(header));
//     header.time = mtime;
//     header.xflags = 0; // 设置为 best compression
//     header.os = 255;     // OS = 0 (FAT/Windows) 与 buildah 一致
//     deflateSetHeader(&zs, &header);

//     const size_t bufSize = 16384;
//     std::vector<char> inBuffer(bufSize);
//     std::vector<char> outBuffer(bufSize);

//     zs.avail_in = 0;

//     while (inputStream.good()) {
//         inputStream.read(inBuffer.data(), bufSize);
//         zs.next_in = reinterpret_cast<Bytef*>(inBuffer.data());
//         zs.avail_in = static_cast<uInt>(inputStream.gcount());

//         int flush = inputStream.eof() ? Z_FINISH : Z_NO_FLUSH;

//         do {
//             zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());
//             zs.avail_out = bufSize;

//             ret = deflate(&zs, flush);
//             if (ret == Z_STREAM_ERROR) throw std::runtime_error("deflate failed");

//             size_t have = bufSize - zs.avail_out;
//             outputStream.write(outBuffer.data(), have);
//         } while (zs.avail_out == 0);
//     }

//     deflateEnd(&zs);
// }
/**
 * @brief 将输入流中的 gzip 数据解压缩并输出到输出流
 * @param inputStream 输入流，提供 gzip 格式数据
 * @param outputStream 输出流，接收解压缩后的数据
 */
void gzip_decompress(std::istream& inputStream, std::ostream& outputStream) {
    try {
        // 使用 gzip 解压缩过滤器
        io::filtering_stream<io::input> filterStream;
        filterStream.push(io::gzip_decompressor());
        filterStream.push(inputStream);

        // 将过滤流数据写入输出流，完成解压缩
        outputStream << filterStream.rdbuf();

        std::cout << "Decompression successful." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during decompression: " << e.what() << std::endl;
    }
}
/**
 * @brief 创建新的压缩算法对象
 * 
 * @param name 算法名称
 * @param nontrivialBaseVariantName 基础变体名称
 * @param prefix 算法标识前缀
 * @param decompressor 解压缩函数指针
 * @param compressor 压缩函数指针
 * @return std::shared_ptr<Algorithm> 返回算法对象的智能指针
 */
std::shared_ptr<Algorithm> NewAlgorithm(string name,std::string nontrivialBaseVariantName,std::vector<uint8_t> prefix,DecompressorFunc decompressor,CompressorFunc compressor){
    auto algorithm=std::make_shared<Algorithm>();
    algorithm->name=name;
    algorithm->baseVariantName=nontrivialBaseVariantName;
    algorithm->prefix=prefix;
    algorithm->decompressor=decompressor;
    algorithm->compressor=compressor;
    return algorithm;   
}
/**
 * @brief 获取算法的压缩函数
 * 
 * @param algorithm 算法对象
 * @return CompressorFunc 返回压缩函数指针
 */
CompressorFunc AlgorithmCompressor(std::shared_ptr<Algorithm> algorithm){
    return algorithm->compressor;
}
/**
 * @brief 获取算法的解压缩函数
 * 
 * @param algorithm 算法对象
 * @return DecompressorFunc 返回解压缩函数指针
 */
DecompressorFunc AlgorithmDecompressor(std::shared_ptr<Algorithm> algorithm){
    return algorithm->decompressor;    
}
