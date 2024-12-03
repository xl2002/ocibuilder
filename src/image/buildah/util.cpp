#include "image/buildah/util.h"
bool isReferenceSomething(
    std::shared_ptr<ImageReference_interface> ref,
    std::shared_ptr<SystemContext> sc,
    std::function<bool(std::shared_ptr<ImageReference_interface>,std::shared_ptr<SystemContext>)> func
){
    return false;
}
bool isReferenceBlocked(std::shared_ptr<ImageReference_interface> ref,std::shared_ptr<SystemContext> sc){
    if(ref!=nullptr && ref->Transport()!=nullptr){
        auto name=ref->Transport()->Name();
        if(name=="docker"){
            return isReferenceSomething(ref,sc,isReferenceBlocked);
        }

    }
    return false;
}

std::map<std::string,std::string> copyStringStringMap(std::map<std::string,std::string> src){
    std::map<std::string,std::string> dst;
    for(auto& e:src){
        dst[e.first]=e.second;
    }
    return dst;
}