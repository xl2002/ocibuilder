#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <iostream>
#include <sstream>

int main() {
    std::string data = "Hello, Boost and zlib!";
    std::stringstream compressed, decompressed;

    // 压缩数据
    {
        boost::iostreams::filtering_stream<boost::iostreams::output> out;
        out.push(boost::iostreams::gzip_compressor());
        out.push(compressed);
        out << data;
    }

    // 解压数据
    {
        boost::iostreams::filtering_stream<boost::iostreams::input> in;
        in.push(boost::iostreams::gzip_decompressor());
        in.push(compressed);
        decompressed << in.rdbuf();
    }

    std::cout << "Decompressed: " << decompressed.str() << std::endl;
    return 0;
}