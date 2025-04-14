#include "image/buildah/push.h"
#include "utils/common/error.h"
#include "image/types/blobcache.h"
/**
 * @brief 创建用于查找Blob缓存引用的函数
 * 
 * @param directory 缓存目录路径
 * @param compression 层压缩配置
 * @return LookupReferenceFunc 返回一个查找引用函数
 * 
 * @details 该函数返回一个lambda函数，用于查找或创建Blob缓存引用。
 * 如果directory为空字符串，则直接返回原始引用；否则尝试创建新的Blob缓存引用。
 * 
 * @note 如果创建缓存引用失败会抛出myerror异常
 * @see NewBlobCache
 */
LookupReferenceFunc cacheLookupReferenceFunc(std::string directory, std::shared_ptr<LayerCompression> compression){
    return [=](std::shared_ptr<ImageReference_interface>ref)->std::shared_ptr<ImageReference_interface>{
        if(directory==""){
            return ref;
        }
        try{
            auto new_ref=NewBlobCache(ref,directory, compression);
            return new_ref;
        }catch(const myerror& e){
            throw myerror("creating blob cache: "+std::string(e.what()));
        }
    };
}
