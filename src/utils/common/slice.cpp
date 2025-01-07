#include "utils/common/slice.h"

std::shared_ptr<Slice> NewSlice(std::vector<std::string> values) {
    auto re = std::make_shared<Slice>();
    re->Values=values;
    return re;
}
void Slice::Set(std::vector<std::string> values){
    Values=values;
}

std::vector<std::string> Slice::Get(){
    if(Values.size()==0){
        return std::vector<std::string>();
    }
    return Values;
}