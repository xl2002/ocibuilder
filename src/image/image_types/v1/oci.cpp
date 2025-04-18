#include <image/image_types/v1/oci.h>
/**
 * @brief 从manifestBlob（byte数组）中创建OCI1
 * 
 * @param manifestBlob 
 * @return std::shared_ptr<OCI1> 
 */
std::shared_ptr<OCI1> OCI1FromManifest(std::vector<uint8_t> manifestBlob){
    std::string manifeststr=vectorToString(manifestBlob);
    auto oci1=unmarshal<OCI1>(manifeststr);
    return std::make_shared<OCI1>(oci1);
}

/**
 * @brief 从blob数据创建OCI1对象
 * @param manblob 镜像manifest的二进制数据
 * @param mt 媒体类型(未使用)
 * @return std::shared_ptr<OCI1> 返回创建的OCI1对象指针
 */
std::shared_ptr<OCI1> FromBlob(std::vector<uint8_t> manblob, std::string mt){
    return OCI1FromManifest(manblob);
}
/**
 * @brief 返回镜像的层信息
 * 
 * @return std::vector<LayerInfo> 
 */
std::vector<LayerInfo> OCI1::LayerInfos(){
    std::vector<LayerInfo> blobs;
    for(auto layer: this->Layers){
        auto blob=BlobInfoFromOCI1Descriptor(std::make_shared<Descriptor>(layer));
        LayerInfo info;
        info.BlobInfo=blob;
        blobs.push_back(info);
    }
    return blobs;
}
/**
 * @brief 从OCI1描述符创建BlobInfo对象
 * @param desc OCI1描述符指针
 * @return std::shared_ptr<::BlobInfo> 返回创建的BlobInfo对象指针
 */
std::shared_ptr<::BlobInfo> BlobInfoFromOCI1Descriptor(std::shared_ptr<Descriptor> desc){
    auto ret=std::make_shared<::BlobInfo>();
    ret->Digest=std::make_shared<Digest>(desc->Digests);
    ret->Size=desc->Size;
    ret->URLs=desc->URLs;
    ret->Annotations=desc->Annotations;
    ret->MediaType=desc->MediaType;
    return ret;
}
