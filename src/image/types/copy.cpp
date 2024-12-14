
#include "image/types/copy.h"
/**
 * @brief // Image将图像从srcRef复制到destRef，使用policyContext来验证源图像的可接受性。 
 *  它返回写入新镜像副本的清单。
 * 
 * @param destRef 
 * @param srcRef 
 * @param registry 
 * @param copyOptions 
 */
std::vector<uint8_t> Image(std::shared_ptr<PolicyContext>policyContext,std::shared_ptr<ImageReference_interface> destRef,std::shared_ptr<ImageReference_interface> srcRef,std::shared_ptr<ImageReference_interface> registry,std::shared_ptr<copy::Options> copyOptions){
    // 1. 建立镜像够简单的缓存目录
    // publicDest, err := destRef.NewImageDestination(ctx, options.DestinationCtx)

    // 2. 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
    // publicRawSource, err := srcRef.NewImageSource(ctx, options.SourceCtx)

    // 3. 将缓存中的镜像传输到镜像库，并且就行gzip压缩
    // single, err := c.copySingleImage(ctx, c.unparsedToplevel, nil, copySingleImageOptions{requireCompressionFormatMatch: requireCompressionFormatMatch})
    
    // 4. 更新镜像库的index.json

    // 5. 返回写入新镜像副本的清单

    return std::vector<uint8_t>();
}
/**
 * @brief 将缓存中的镜像层传输到镜像库，并且就行gzip压缩
 * 
 * @param unparsedImage 
 * @param targetInstance 
 * @param opts 
 * @return std::shared_ptr<copySingleImageResult> 
 */
std::shared_ptr<copySingleImageResult> copier::copySingleImage(std::shared_ptr<UnparsedImage> unparsedImage,std::shared_ptr<Digest> targetInstance,std::shared_ptr<copySingleImageOptions> opts){
    // 1. 构建imagecopier结构，注意类的成员变量已不同

    // 2. 将缓存中的镜像层传输到镜像库，并且就行gzip压缩
    // compressionAlgos, err := ic.copyLayers(ctx)

    // 3. 更新manifest和config（按理来说config不会变化），并将manifest的layer存储到oci库

    // 4. 返回copySingleImageResult
    return nullptr;
}

/**
 * @brief 将缓存中的镜像层传输到镜像库，并且就行gzip压缩
 * 
 * @return std::shared_ptr<Algorithm> 
 */
std::shared_ptr<Algorithm> imageCopier::copyLayers(){
    // 1. 获得当前镜像所有层的信息

    // 2. 将序列化的manifest转化为oci格式的manifest,继续返回manifest的info

    // 3. 循环传输每一个层到镜像库，并且就行gzip压缩。gzip已经实现，详细见Algorithm

    // 4. 返回compressionAlgos
}
/**
 * @brief 将缓存中单个层传输到镜像库，并且就行gzip压缩
 * 
 * @param srcInfo 
 * @param toEncrypt 
 * @param layerIndex 
 * @param srcRef 
 * @param emptyLayer 
 * @return std::tuple<std::shared_ptr<BlobInfo>,std::shared_ptr<Digest>> 
 */
std::tuple<std::shared_ptr<BlobInfo>,std::shared_ptr<Digest>> imageCopier::copyLayer(std::shared_ptr<BlobInfo> srcInfo,bool toEncrypt,int layerIndex,std::shared_ptr<Named_interface> srcRef,bool emptyLayer){
    
    // 1. 读取镜像层为文件流
    
    // 2. 构建gzip压缩算法，对文件流进行压缩

    // 3. 计算压缩后镜像层的sha256值

    // 4. 将压缩后的镜像层传输到镜像库
    
    return std::make_tuple(std::make_shared<BlobInfo>(),std::make_shared<Digest>());
}
// /**
//  * @brief 
//  * 
//  * @param layerCompressionChangeSupported 
//  * @return std::shared_ptr<bpCompressionStepData> 
//  */
// std::shared_ptr<bpCompressionStepData> imageCopier::bpcPreserveOriginal(std::shared_ptr<bpDetectCompressionStepData>detect,bool layerCompressionChangeSupported){
//     return nullptr;
// }