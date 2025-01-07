#include "image/types/internal/types.h"
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <sstream>
#include <iostream>
#include <string>

namespace io = boost::iostreams;

/**
 * @brief 将输入流数据压缩为 gzip 格式并输出到输出流
 * @param inputStream 输入流，提供原始数据
 * @param outputStream 输出流，接收压缩后的数据
 */
void gzip_compress(std::istream& inputStream, std::ostream& outputStream) {
    try {
        // 使用 gzip 压缩过滤器
        io::filtering_stream<io::output> filterStream;
        filterStream.push(io::gzip_compressor());
        filterStream.push(outputStream);

        // 将输入流数据写入过滤流，完成压缩
        filterStream << inputStream.rdbuf();
        filterStream.reset(); // 确保所有数据被刷新到输出流

        // std::cout << "Compression successful." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error during compression: " << e.what() << std::endl;
    }
}

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