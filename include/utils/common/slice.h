#if !defined(UTILS_COMMON_SLICE_H)
#define UTILS_COMMON_SLICE_H
#include <string>
#include <vector>
#include <memory>
struct Slice{
    std::vector<std::string> Values;
    struct {
        std::shared_ptr<bool> Append=std::make_shared<bool>(false);
    }Attributes;
    void Set(std::vector<std::string> values);
    std::vector<std::string> Get();
};
std::shared_ptr<Slice> NewSlice(std::vector<std::string> values);


#endif // ATTRIBUTEDSTRING_SLICE_H)
