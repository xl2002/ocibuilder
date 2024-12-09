#include <image/image_types/v1/oci.h>
/**
 * @brief 从manifestBlob（byte数组）中创建OCI1
 * 
 * @param manifestBlob 
 * @return std::shared_ptr<OCI1> 
 */
std::shared_ptr<OCI1> OCI1FromManifest(std::vector<uint8_t> manifestBlob){
    return nullptr;
}

std::shared_ptr<OCI1> FromBlob(std::vector<uint8_t> manblob, std::string mt){
    return OCI1FromManifest(manblob);
}
/**
 * @brief 返回镜像的层信息
 * 
 * @return std::vector<LayerInfo> 
 */
std::vector<LayerInfo> OCI1::LayerInfos(){
    return {};
}
std::shared_ptr<::BlobInfo> BlobInfoFromOCI1Descriptor(std::shared_ptr<Descriptor> desc){
    auto ret=std::make_shared<::BlobInfo>();
    ret->Digest=std::make_shared<Digest>(desc->Digests);
    ret->Size=desc->Size;
    ret->URLs=desc->URLs;
    ret->Annotations=desc->Annotations;
    ret->MediaType=desc->MediaType;
    return ret;
}