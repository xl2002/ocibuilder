
#include "image/types/copy.h"
#include "image/buildah/image.h"
#include "image/types/internal/types.h"
#include "utils/common/go/string.h"
#include "storage/storage/images.h"
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
    // destRef为storageReference类型，dest为storageImageDestination类型
    auto dest=destRef->NewImageDestination(copyOptions->destinationCtx);
    // 2. 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
    // publicRawSource, err := srcRef.NewImageSource(ctx, options.SourceCtx)
    // srcRef为containerImageRef类型，rawSource为containerImageSource类型
    auto rawSource=srcRef->NewImageSource(copyOptions->sourceCtx);
    // 3. 将缓存中的镜像传输到镜像库，并且就行gzip压缩
    // single, err := c.copySingleImage(ctx, c.unparsedToplevel, nil, copySingleImageOptions{requireCompressionFormatMatch: requireCompressionFormatMatch})
    auto c=std::make_shared<copier>();
    c->policyContext=policyContext;
    c->dest=dest;
    c->rawSource=rawSource;
    c->options=copyOptions;
    auto unparsedToplevel=std::make_shared<UnparsedImage>();
    unparsedToplevel->src=rawSource;
    auto copySingleImageoptions=std::make_shared<copySingleImageOptions>();
    copySingleImageoptions->requireCompressionFormatMatch=true;
    auto single=c->copySingleImage(c->unparsedToplevel,nullptr,copySingleImageoptions);
    // 4. 更新镜像库的index.json
    auto containerimage=std::dynamic_pointer_cast<containerImageSource>(rawSource);
    if(containerimage==nullptr){
        std::cerr<<"containerimage is nullptr"<<std::endl;
        return std::vector<uint8_t>();
    }
    std::string indexpath=containerimage->store->GetImageStoragePath()+"/index.json";
    if(!boost::filesystem::exists(indexpath)){
        std::cout<<"index.json is not exist"<<std::endl;
        return std::vector<uint8_t>();
    }
    boost::filesystem::ifstream indexfile(indexpath,std::ios::binary);
    // 使用 std::ostringstream 将流的内容读取到字符串
    std::ostringstream buffer;
    buffer << indexfile.rdbuf();  // 读取整个文件内容
    std::string fileContent = buffer.str();  // 转换为 std::string
    storage::index index;
    if(!fileContent.empty()){
        index=unmarshal<storage::index>(fileContent);
    }
    indexfile.close();
    auto newmanifest=std::make_shared<storage::manifest>();
    newmanifest->mediaType=single->manifestMIMEType;
    newmanifest->digest=single->manifestDigest->String();
    newmanifest->size=single->manifest.size();
    auto destimage=std::dynamic_pointer_cast<storageImageDestination>(dest);
    auto tagref=std::dynamic_pointer_cast<taggedReference>(destimage->imageRef->named);
    if(destimage!=nullptr&&tagref!=nullptr){
        newmanifest->annotations["org.opencontainers.image.ref.name"]=tagref->String();
    }
    index.manifests.push_back(*newmanifest);
    index.schemaVersion=2;
    // index.manifests->annotations["org.opencontainers.image.ref.name"]=tagref->String();
    // 5. 返回写入新镜像副本的清单
    std::string newindexcontent=marshal<storage::index>(index);
    std::ofstream newindexfile(indexpath,std::ios::out|std::ios::trunc);
    newindexfile<<newindexcontent;
    newindexfile.close();
    // 更新layerstore的layer.json

    
    return single->manifest;
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
    std::shared_ptr<imageCopier> ic = std::make_shared<imageCopier>();
    ic->src = rawSource;
    ic->c=std::make_shared<copier>(*this);
    auto manifestupdates=std::make_shared<ManifestUpdateOptions>();
    auto info=std::make_shared<ManifestUpdateInformation>();
    info->Destination=this->dest;
    manifestupdates->InformationOnly=info;
    ic->manifestUpdates=manifestupdates;
    ic->diffIDsAreNeeded = false;
    // 2. 将缓存中的镜像层传输到镜像库，并且就行gzip压缩
    // compressionAlgos, err := ic.copyLayers(ctx)
    std::shared_ptr<Algorithm> compressionAlgo = ic->copyLayers();
    if (!compressionAlgo) {
        return nullptr;
    }
    // 3. 更新manifest和config（按理来说config不会变化），并将manifest的layer存储到oci库
    auto pendingImage=std::dynamic_pointer_cast<containerImageSource>(rawSource);
    auto manifest=pendingImage->OCI1FromManifest();
    auto config= pendingImage->config;
    //复制config到库
    // auto configdigest=FromBytes(config);
    // if(configdigest->String()!=pendingImage->configDigest->String()){//config可能已经变化
    //     std::cerr<<"config changed"<<std::endl;
    //     return nullptr;
    // }
    std::string storagepath=pendingImage->store->GetImageStoragePath()+"/blobs/sha256/";
    std::string newname;
    std::string configpath=storagepath+"config.json";
    if(boost::filesystem::exists(configpath)){
        std::cerr<<"config is exist"<<std::endl;
        return nullptr;
    }
    boost::filesystem::ofstream file(configpath,std::ios::binary|std::ios::trunc|std::ios::out);
    file.write(reinterpret_cast<const char*>(config.data()),config.size());
    file.close();
    auto configdigest=Fromfile(configpath);
    newname=storagepath+configdigest->Encoded();
    boost::filesystem::rename(configpath, newname);



    //更新manifest
    auto blobsinfo_gzip=ic->manifestUpdates->InformationOnly->LayerInfos;
    if(blobsinfo_gzip.size()!=manifest->Layers.size()){
        std::cerr<<"manifest num is not match"<<std::endl;
        return nullptr;
    }
    for(int i=0;i<manifest->Layers.size();i++){
        manifest->Layers[i].Digests=*blobsinfo_gzip[i].Digest;
        manifest->Layers[i].Size=blobsinfo_gzip[i].Size;
        manifest->Layers[i].MediaType=blobsinfo_gzip[i].MediaType;
    }

    //复制manifest到库
    auto manifestbytes=marshal(*manifest);//manifest为指针，不能直接解析
    // auto manifestdigest=FromString(manifestbytes);//已经计算好的最终manifest的哈希值
    std::string manifestpath=storagepath+"manifest.json";
    if(boost::filesystem::exists(manifestpath)){
        std::cerr<<"manifest is exist"<<std::endl;
        return nullptr;
    }
    boost::filesystem::ofstream file2(manifestpath,std::ios::binary|std::ios::trunc|std::ios::out);
    file2.write(reinterpret_cast<const char*>(manifestbytes.data()),manifestbytes.size());
    file2.close();
    auto manifestdigest=Fromfile(manifestpath);
    newname=storagepath+manifestdigest->Encoded();
    boost::filesystem::rename(manifestpath, newname);
    // 4. 返回copySingleImageResult
    std::shared_ptr<copySingleImageResult> result = std::make_shared<copySingleImageResult>();
    result->manifest=stringToVector(manifestbytes);
    result->manifestMIMEType=pendingImage->manifestType;
    result->manifestDigest=manifestdigest;
    result->compressionAlgorithms.push_back(compressionAlgo);
    return result;
}

/**
 * @brief 将缓存中的镜像层传输到镜像库，并且就行gzip压缩
 * 
 * @return std::shared_ptr<Algorithm> 
 */
std::shared_ptr<Algorithm> imageCopier::copyLayers(){
    // 1. 获得当前镜像所有层的信息
    auto srcinfos=this->src->LayerInfos();
    auto src=std::dynamic_pointer_cast<containerImageSource>(this->src);
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
    // auto copyimage=this->c;
    auto copysource=std::dynamic_pointer_cast<containerImageSource>(this->src);
    if(copysource==nullptr){
        std::cout<<"copysource is nullptr"<<std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    std::string tmppath=copysource->path;
    boost::filesystem::path layerpath(tmppath+"/"+srcInfo->Digest->Encoded());
    if(!boost::filesystem::exists(layerpath)){
        std::cout<<"layerpath is not exist"<<std::endl;
    }
    boost::filesystem::ifstream layerfile(layerpath,std::ios::binary);

    // 2. 构建gzip压缩算法，对文件流进行压缩
    auto alg=NewAlgorithm("gzip","gzip",std::vector<uint8_t>(),gzip_decompress,gzip_compress);
    // std::string tempath=copysource->store->GetImageStoragePath()+"/blobs/sha256/layer.gz";
    std::ostringstream outputStream;
    alg->compressor(layerfile,outputStream);
    std::string gzipblob=outputStream.str();
    // auto buffer=stringToVector(gzipblob);

    // 3. 计算压缩后镜像层的sha256值
    auto digest=FromString(gzipblob);
    auto size=gzipblob.size();

    // 4. 将压缩后的镜像层传输到镜像库
    auto blobinfo=std::make_shared<BlobInfo>();
    blobinfo->Digest=digest;
    blobinfo->Size=size;
    std::cout<<"copying layer: "<<blobinfo->Digest->String()<<std::endl;
    blobinfo->MediaType=srcInfo->MediaType+"+gzip";
    blobinfo->CompressionAlgorithm=alg;
    std::string storagelayerpath=copysource->store->GetImageStoragePath()+"/blobs/sha256/"+blobinfo->Digest->Encoded();
    boost::filesystem::path p(storagelayerpath);
    if(boost::filesystem::exists(p)){
        std::cout<<"layer "<< storagelayerpath<<"already exists"<<std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    boost::filesystem::ofstream bloblayer(p,std::ios::binary);
    if(!bloblayer){
        std::cerr<<"Failed to open file: " << storagelayerpath << std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    bloblayer<<gzipblob;
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
