
#include "image/types/copy.h"
#include "image/buildah/image.h"
#include "image/types/internal/types.h"
#include "utils/common/go/string.h"
#include "storage/storage/images.h"
#include "image/image_types/v1/annotations.h"
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
    // index.addManifest(c,single);//更新index.json
    auto newmanifest=std::make_shared<storage::manifest>();
    newmanifest->mediaType=single->manifestMIMEType;
    newmanifest->digest=single->manifestDigest->String();
    newmanifest->size=single->manifest.size();
    auto destimage=std::dynamic_pointer_cast<storageImageDestination>(dest);
    auto tagref=std::dynamic_pointer_cast<taggedReference>(destimage->imageRef->named);
    std::string newname=tagref->String();
    if(destimage!=nullptr&&tagref!=nullptr){
        newmanifest->annotations["org.opencontainers.image.ref.name"]=newname;
    }
    //检测manifest中是否已经有相同的镜像，如果有，删除
    // index.manifests.erase(std::remove_if(index.manifests.begin(), index.manifests.end(), 
    //     [newname](const storage::manifest& manifest) {
    //         auto it = manifest.annotations.find("org.opencontainers.image.ref.name");
    //         return it != manifest.annotations.end() && it->second == newname;
    //         }), index.manifests.end());
    auto m=std::find_if(index.manifests.begin(), index.manifests.end(), 
        [newname](const storage::manifest& manifest) {
            auto it = manifest.annotations.find("org.opencontainers.image.ref.name");
            return it != manifest.annotations.end() && it->second == newname;
            });
    if(m!=index.manifests.end()){
        newmanifest->historyTags=m->historyTags;
        newmanifest->historyTags.push_back(m->digest);
        index.manifests.erase(m);
    }
    index.manifests.push_back(*newmanifest);
    index.schemaVersion=2;
    // index.manifests->annotations["org.opencontainers.image.ref.name"]=tagref->String();
    // 5. 返回写入新镜像副本的清单
    std::string newindexcontent=marshal<storage::index>(index);
    std::ofstream newindexfile(indexpath,std::ios::out|std::ios::trunc);
    newindexfile<<newindexcontent;
    newindexfile.close();
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
    // auto config= pendingImage->config;
    //复制config到库
    if(pendingImage->SaveConfig()){
        std::cout<<"save config success"<<std::endl;
    }

    //更新manifest
    auto blobsinfo_gzip=ic->manifestUpdates->InformationOnly->LayerInfos;//获得压缩层的信息
    if(blobsinfo_gzip.size()!=manifest->Layers.size()){
        std::cerr<<"manifest num is not match"<<std::endl;
        return nullptr;
    }
    for(int i=0;i<manifest->Layers.size();i++){
        manifest->Layers[i].Digests=*blobsinfo_gzip[i].Digest;
        manifest->Layers[i].Size=blobsinfo_gzip[i].Size;
        manifest->Layers[i].MediaType=blobsinfo_gzip[i].MediaType;
    }
    // if(manifest->Annotations[AnnotationBaseImageDigest]==""){
    //     manifest->Annotations[AnnotationBaseImageDigest]=manifest->Layers[0].Digests.String();
    // }
    //复制manifest到库
    std::string manifestbytes=marshal<OCI1>(*manifest);//manifest为指针，不能直接解析
    auto manifestdigest=pendingImage->UploadManifest(manifestbytes);//保存manifest到库
    if(manifestdigest==nullptr){
        std::cerr<<"upload manifest failed"<<std::endl;
    }
    // 4. 返回copySingleImageResult
    std::shared_ptr<copySingleImageResult> result = std::make_shared<copySingleImageResult>();
    result->manifest=stringToVector(manifestbytes);
    result->manifestMIMEType=pendingImage->manifestType;
    result->manifestDigest=manifestdigest;
    result->compressionAlgorithms.push_back(compressionAlgo);

    auto removenum=boost::filesystem::remove_all(pendingImage->path);//删除缓存的buildah文件夹
    if(removenum!=0){
        std::cout<<"success remove tar tmp directory: "<< pendingImage->path<<std::endl;
    }
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
    std::string storagelayerpath=copysource->store->GetImageStoragePath()+"/blobs/sha256/blobs";
    boost::filesystem::ofstream bloblayer(storagelayerpath,std::ios::binary|std::ios::trunc);
    if(!bloblayer){
        std::cerr<<"Failed to open file: " << storagelayerpath << std::endl;
        return std::make_tuple(nullptr,nullptr);
    }
    bloblayer<<gzipblob;
    bloblayer.close();
    auto BlobDigest=Fromfile(storagelayerpath);
    std::string newBlobname=copysource->store->GetImageStoragePath()+"/blobs/sha256/"+BlobDigest->Encoded();
    if(boost::filesystem::exists(newBlobname)){
        boost::filesystem::remove_all(storagelayerpath);
    }else{
        boost::filesystem::rename(storagelayerpath, newBlobname);
    }
    // auto digest=FromString(gzipblob);
    auto size=gzipblob.size();
    std::cout<<"success copying layer: "<<BlobDigest->String()<<std::endl;
    // 4. 将压缩后的镜像层传输到镜像库
    auto blobinfo=std::make_shared<BlobInfo>();
    blobinfo->Digest=BlobDigest;
    blobinfo->Size=size;
    blobinfo->MediaType=srcInfo->MediaType+"+gzip";
    blobinfo->CompressionAlgorithm=alg;

    // 关闭文件流
    layerfile.close();
    // bloblayer.close();
    return std::make_tuple(blobinfo,BlobDigest);
}
