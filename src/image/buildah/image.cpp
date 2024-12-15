#include "image/buildah/buildah.h"
#include "image/buildah/image.h"
#include "utils/common/go/file.h"
#include "utils/common/go/string.h"
#include "utils/common/json.h"
std::shared_ptr<containerImageRef> Builder::makeContainerImageRef(std::shared_ptr<CommitOptions> options){

    return nullptr;
}
std::shared_ptr<ImageTransport_interface> containerImageRef::Transport(){
    return nullptr;
}
std::string containerImageRef::StringWithinTransport(){
    return "";
}
std::string containerImageRef::PolicyConfigurationIdentity(){
    return "";
}
std::vector<std::string> containerImageRef::PolicyConfigurationNamespaces(){
    return {};
}
std::shared_ptr<Named_interface> containerImageRef::DockerReference(){
    return nullptr;
}
std::shared_ptr<Image_interface>  containerImageRef::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
/**
 * @brief 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
 * 
 * @return std::shared_ptr<ImageSource_interface> 
 */
std::shared_ptr<ImageSource_interface> containerImageRef::NewImageSource(std::shared_ptr<SystemContext>sys){
    // 1. 得到整个镜像所有层的ID

    // 2. 构建整个镜像的config和manifest(默认的)
    
    // 3. 将原目录下diff文件夹下的内容复制到目的缓存目录下，如果目的目录不存在则新建目录
    //  int64_t Copy_directory(const fs::path& source, const fs::path& destination)复制目录的函数已写好，并且返回数据大小

    // 4. 将缓存目的目录下的内容打包成tar文件，返回tar文件流

    // 5. 计算tar文件的sha256值，并且重命名tar文件

    // 6. 组织历史记录history，构造appendHistory函数

    // 7. 将新构建的oci config反序列化为byte，记住marshal函数返回string，需要转化为vector<uint8_t>,函数在
    // std::vector<uint8_t> stringToVector(const std::string& str)

    // 8. 更改manifest内容，反序列化为byte，记住这个manifest不是最终的manifest，液位这个时候镜像层没有gzip压缩

    return nullptr;
}
/**
 * @brief 创建默认的oci镜像的config和manifest
 * 
 * @return std::tuple<std::shared_ptr<::Image>,std::shared_ptr<Manifest>> 
 */
std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> containerImageRef::createConfigsAndManifests(){
    // 不用管docker格式的
    return std::make_tuple(nullptr,nullptr);
}
/**
 * @brief 只判断oci镜像的mediatype
 * 
 * @param what 
 * @param layerCompression 
 * @return std::string 
 */
std::string computeLayerMIMEType(std::string what,std::shared_ptr<Compression> layerCompression){
    return "";
}

std::shared_ptr<ImageDestination_interface> containerImageRef::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
std::shared_ptr<ImageReference_interface> containerImageSource::Reference(){
    return this->ref;
}
std::tuple<std::vector<uint8_t>,std::string> containerImageSource::GetManifest(std::shared_ptr<Digest> instanceDigest){
    return std::make_tuple(this->manifest,this->manifestType);
}
/**
 * @brief 返回镜像的层信息
 * 
 * @return std::shared_ptr<BlobInfo> 
 */
std::vector<BlobInfo>  containerImageSource::LayerInfos(){
    auto oci=this->OCI1FromManifest();
    auto layers=oci->LayerInfos();
    auto blobs = std::vector<BlobInfo>();
    for(auto layer:layers){
        blobs.push_back(*layer.BlobInfo);
    }
    return blobs;
}
std::shared_ptr<OCI1>containerImageSource::OCI1FromManifest(){
    std::string manifeststr=vectorToString(this->manifest);
    auto oci1=unmarshal<OCI1>(manifeststr);
    return std::make_shared<OCI1>(oci1);
}