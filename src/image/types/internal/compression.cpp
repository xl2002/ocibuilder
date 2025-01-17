#include "image/types/internal/types.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
// #include <boost/iostreams.hpp>
namespace io = boost::iostreams;

/**
 * @brief 将输入流数据压缩为 gzip 格式并输出到输出流
 * @param inputStream 输入流，提供原始数据
 * @param outputStream 输出流，接收压缩后的数据
 */
void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
    try {
        // std::chrono::duration<double> duration;
        // std::chrono::high_resolution_clock::time_point start,end;
        // start=std::chrono::high_resolution_clock::now();
        // 使用 gzip 压缩过滤器
        io::filtering_stream<io::output> filterStream;
        filterStream.push(io::gzip_compressor());
        filterStream.push(outputStream);

        // 将输入流数据写入过滤流，完成压缩
        filterStream << inputStream.rdbuf();
        filterStream.reset(); // 确保所有数据被刷新到输出流

        // end=std::chrono::high_resolution_clock::now();
        // duration=end-start;
        // std::cout << "Compression time: " << duration.count() << " s" << std::endl;
        // std::cout << "Compression successful." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during compression: " << e.what() << std::endl;
    }
}
// void compress_block(const CompressionBlock& block, std::ostream& outputStream) {
//     try {
//         io::filtering_stream<io::output> filterStream;
//         filterStream.push(io::gzip_compressor());
//         filterStream.push(outputStream);

//         // 将数据写入压缩流
//         filterStream.write(block.data.data(), block.size);
//         filterStream.flush(); // 确保数据被写入到输出流

        
//     } catch (const std::exception& e) {
//         std::cerr << "Error during block compression: " << e.what() << std::endl;
//     }
// }

// void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
//     try {
//         std::chrono::duration<double> duration;
//         std::chrono::high_resolution_clock::time_point start,end;
//         start=std::chrono::high_resolution_clock::now();

//         // 分块大小设置为 1MB
//         const size_t block_size = 1024 * 1024;

//         // 创建 Boost 的线程池
//         boost::asio::thread_pool pool;

//         // 计算总的输入数据块
//         std::vector<CompressionBlock> blocks;

//         // 读取输入流并分块
//         char buffer[block_size];
//         while (inputStream.read(buffer, block_size) || inputStream.gcount() > 0) {
//             size_t bytes_read = inputStream.gcount();
//             CompressionBlock block;
//             block.data.resize(bytes_read);
//             std::copy(buffer, buffer + bytes_read, block.data.begin());
//             block.size = bytes_read;

//             blocks.push_back(std::move(block));
//         }

//         // 为每个数据块创建线程进行压缩
//         for (const auto& block : blocks) {
//             boost::asio::post(pool, [&]() {
//                 compress_block(block, outputStream);
//             });
//         }

//         // 等待所有任务完成
//         pool.join();

//         end=std::chrono::high_resolution_clock::now();
//         duration=end-start;
//         std::cout << "Compression time: " << duration.count() << " s" << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << "Error during compression: " << e.what() << std::endl;
//     }
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
std::shared_ptr<Algorithm> NewAlgorithm(string name,std::string nontrivialBaseVariantName,std::vector<uint8_t> prefix,DecompressorFunc decompressor,CompressorFunc compressor){
    auto algorithm=std::make_shared<Algorithm>();
    algorithm->name=name;
    algorithm->baseVariantName=nontrivialBaseVariantName;
    algorithm->prefix=prefix;
    algorithm->decompressor=decompressor;
    algorithm->compressor=compressor;
    return algorithm;   
}
CompressorFunc AlgorithmCompressor(std::shared_ptr<Algorithm> algorithm){
    return algorithm->compressor;
}
DecompressorFunc AlgorithmDecompressor(std::shared_ptr<Algorithm> algorithm){
    return algorithm->decompressor;    
}