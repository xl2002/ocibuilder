
#include "image/types/copy.h"
#include "image/buildah/image.h"
#include "image/types/internal/types.h"
#include "utils/common/go/string.h"
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
    auto srcinfos=this->c->rawSource->LayerInfos();
    auto src=std::dynamic_pointer_cast<containerImageSource>(this->c->rawSource);
    auto numLayers=srcinfos.size();

    // 2. 将序列化的manifest转化为oci格式的manifest,继续返回manifest的info
    auto man=FromBlob(src->manifest,src->manifestType);
    auto manifestLayerInfos=man->LayerInfos();
    struct copyLayerData{
        std::shared_ptr<BlobInfo> blobInfo=nullptr;
        std::shared_ptr<Digest> digest=nullptr;
    };
    std::vector<copyLayerData> data;
    // 3. 循环传输每一个层到镜像库，并且就行gzip压缩。gzip已经实现，详细见Algorithm
    for(auto i=0;i<numLayers;i++){
        copyLayerData cld;
        std::tie(cld.blobInfo,cld.digest)=this->copyLayer(std::make_shared<BlobInfo>(srcinfos[i]),false,i,src->Reference(),manifestLayerInfos[i].EmptyLayer);
        data.push_back(cld);
    }
    std::vector<BlobInfo> destInfos;
    std::vector<Digest> diffIDs;
    std::shared_ptr<Algorithm> alg;
    for(auto i=0;i<data.size();i++){
        destInfos.push_back(*data[i].blobInfo);
        diffIDs.push_back(*data[i].digest);
        if(data[i].blobInfo->CompressionAlgorithm!=nullptr){
            alg=data[i].blobInfo->CompressionAlgorithm;
        }
    }
    this->manifestUpdates->InformationOnly->LayerInfos=destInfos;
    this->manifestUpdates->InformationOnly->LayerDiffIDs=diffIDs;

    // 4. 返回compressionAlgos
    return alg;
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
std::tuple<std::shared_ptr<BlobInfo>,std::shared_ptr<Digest>> imageCopier::copyLayer(std::shared_ptr<BlobInfo> srcInfo,bool toEncrypt,int layerIndex,std::shared_ptr<ImageReference_interface> srcRef,bool emptyLayer){
    
    // 1. 读取镜像层的文件流
    auto copyimage=this->c;
    auto copysource=std::dynamic_pointer_cast<containerImageSource>(copyimage->rawSource);
    if(copysource==nullptr){
        std::cout<<"copysource is nullptr"<<std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    std::string tmppath=copysource->path;
    boost::filesystem::path layerpath(tmppath+"/"+srcInfo->Digest->digest);
    if(!boost::filesystem::exists(layerpath)){
        std::cout<<"layerpath is not exist"<<std::endl;
    }
    boost::filesystem::ifstream layerfile(layerpath,std::ios::binary);

    // 2. 构建gzip压缩算法，对文件流进行压缩
    auto alg=NewAlgorithm("gzip","gzip",std::vector<uint8_t>(),gzip_decompress,gzip_compress);
    std::ostringstream outputStream;
    alg->compressor(layerfile,outputStream);
    std::string gzipblob=outputStream.str();
    auto buffer=stringToVector(gzipblob);

    // 3. 计算压缩后镜像层的sha256值
    auto digest=FromBytes(buffer);
    auto size=buffer.size();

    // 4. 将压缩后的镜像层传输到镜像库
    auto blobinfo=std::make_shared<BlobInfo>();
    blobinfo->Digest=digest;
    blobinfo->Size=size;
    std::cout<<"copying layer: "<<blobinfo->Digest->String()<<std::endl;
    blobinfo->MediaType=srcInfo->MediaType+"+gzip";
    blobinfo->CompressionAlgorithm=alg;
    std::string storagelayerpath=copysource->store->RunRoot()+"/blobs/sha256/"+blobinfo->Digest->Encoded();
    boost::filesystem::path p(storagelayerpath);
    if(boost::filesystem::exists(p)){
        std::cout<<"layer "<< storagelayerpath<<"already exists"<<std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    boost::filesystem::ofstream bloblayer(p,std::ios::out|std::ios::binary|std::ios::trunc);
    if(!bloblayer){
        std::cerr<<"Failed to open file: " << storagelayerpath << std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    bloblayer.write(reinterpret_cast<const char*>(buffer.data()),size);
    // 检查写入是否成功
    if (!bloblayer) {
        std::cerr << "Error writing to file." << std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    // 关闭文件流
    layerfile.close();
    bloblayer.close();
    return std::make_tuple(blobinfo,digest);
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