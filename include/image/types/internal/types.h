#if !defined(INTERNAL_TYPES_H)
#define INTERNAL_TYPES_H
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
using std::string;
using std::vector;
using std::function;
// CompressorFunc 类型定义
using CompressorFunc = std::function<std::shared_ptr<std::ostream>(
    std::ostream&, const std::map<std::string, std::string>&, int*)>;

// DecompressorFunc 类型定义
using DecompressorFunc = std::function<std::shared_ptr<std::istream>(std::istream&)>;
struct Algorithm  {
	string name ;           
	string baseVariantName ;
	vector<uint8_t> prefix; // Initial bytes of a stream compressed using this algorithm, or empty to disable detection.
	DecompressorFunc decompressor    ;
	CompressorFunc compressor    ;

};

#endif // INTERNAL_TYPES_H
