#if !defined(IMAGE_TYPES_INTERNAL_TYPES_H)
#define IMAGE_TYPES_INTERNAL_TYPES_H
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <iostream>
#include <memory>
#include "image/types/types.h"
// #include "storage/storage/storage_transport.h"
using std::string;
using std::vector;
using std::function;
// CompressorFunc 类型定义
using CompressorFunc = std::function<void(std::istream&, std::ostream&)>;

// DecompressorFunc 类型定义
using DecompressorFunc = std::function<void(std::istream&, std::ostream&)>;
class Algorithm  {
	public:
	string name ;           
	string baseVariantName ;
	vector<uint8_t> prefix; // Initial bytes of a stream compressed using this algorithm, or empty to disable detection.
	DecompressorFunc decompressor    ;
	CompressorFunc compressor    ;
	Algorithm()=default;
	std::string Name() const { return name; }
	std::string BaseVariantName() const { return baseVariantName; }
	std::vector<uint8_t> AlgorithmPrefix() const { return prefix; }
};
// 定义一个压缩块结构
class CompressionBlock {
	public:
    std::vector<char> data;
    size_t size;
};
void gzip_compress(std::istream& inputStream, std::ostream& outputStream);
void gzip_decompress(std::istream& inputStream, std::ostream& outputStream);
std::shared_ptr<Algorithm> NewAlgorithm(string name,std::string nontrivialBaseVariantName,std::vector<uint8_t> prefix,DecompressorFunc decompressor,CompressorFunc compressor);
CompressorFunc AlgorithmCompressor(std::shared_ptr<Algorithm> algorithm);
DecompressorFunc AlgorithmDecompressor(std::shared_ptr<Algorithm> algorithm);
class bpDetectCompressionStepData{
	public:
	bool isCompressed=false;
	std::shared_ptr<Algorithm> format=std::make_shared<Algorithm>();
	DecompressorFunc decompressor;
	string srcCompressorName;
	bpDetectCompressionStepData()=default;
};
using bpcOperation=int;
class LayerCompression;
class bpCompressionStepData{
	public:
	bpcOperation operation;
	std::shared_ptr<LayerCompression>uploadedOperation=std::make_shared<LayerCompression>();
	std::shared_ptr<Algorithm>uploadedAlgorithm=std::make_shared<Algorithm>();
	std::map<std::string, std::string>uploadedAnnotations;
	string srcCompressorName;
	string uploadedCompressorName;
	// std::vector<std::shared_ptr<istream>>closers;
	bpCompressionStepData()=default;
};
// std::shared_ptr<Algorithm> NewAlgorithm(string name);
#endif // INTERNAL_TYPES_H
