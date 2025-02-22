#include "image/buildah/buildah.h"
#include "image/buildah/image.h"
#include "utils/common/go/file.h"
#include "utils/common/go/string.h"
#include "utils/common/json.h"
#include "utils/common/go/file.h"
#include "config/defaut.h"
#include "image/buildah/tar.h"
#include "utils/common/json.h"
#include <chrono>
#include <boost/filesystem.hpp>
std::shared_ptr<containerImageRef> Builder::makeContainerImageRef(std::shared_ptr<CommitOptions> options){
    auto container=this->container;
    std::string manifestType;
    if(options->PreferredManifestType!=""){
        manifestType=options->PreferredManifestType;
    }else{
        options->PreferredManifestType=MediaTypeImageManifest;
        manifestType=options->PreferredManifestType;
    }
    auto oconfig=marshal<v1::Image>(*this->OCIv1);//拿到config的配置
    std::chrono::system_clock::time_point created;
    auto createdBy=this->CreatedBy();
    std::string parent;
    if(!boost::filesystem::is_directory(this->FromImage)){
        parent=NewDigestFromEncoded(std::make_shared<Algorithm_sha256>(Canonical_sha256),this->FromImageID)->String();
    }
    auto ref=std::make_shared<containerImageRef>();
    ref->fromImageName=this->FromImage;
    ref->fromImageID=container->ImageID;
    ref->store=this->store;
    ref->compression=options->Compression;
    ref->names=container->Names;
    ref->containerID=container->ID;
    ref->mountLabel=this->MountLabel;
    ref->layerID=container->LayerID;
    ref->oconfig=stringToVector(oconfig);
    ref->created=created;
    ref->createdBy=createdBy;
    ref->historyComment=this->ImageHistoryComment;
    ref->annotations=this->ImageAnnotations;
    ref->preferredManifestType=manifestType;
    ref->squash=options->Squash;
    ref->confidentialWorkload=options->ConfidentialWorkloadOptions;
    ref->omitHistory=options->OmitHistory;
    ref->emptyLayer=options->EmptyLayer&&!options->Squash &&!options->ConfidentialWorkloadOptions->Convert;
    ref->idMappingOptions=this->IDMappingoptions;
    ref->parent=parent;
    ref->blobDirectory=options->BlobDirectory;
    ref->preEmptyLayers=this->PrependedEmptyLayers;
    ref->postEmptyLayers=this->AppendedEmptyLayers;
    ref->overrideChanges=options->OverrideChanges;
    ref->overrideConfig=options->OverrideConfig;
    ref->extraImageContent=options->ExtraImageContent;
    return ref;
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
void AddcCheck(std::vector<std::string> layers,std::string srcpath,std::string destpath){
    //TODO
    std::string checkpath=destpath+"/check.json";//中间缓存文件
    auto checks=std::make_shared<Check>();//TODO
    checks->version=8;
    for(auto layer:layers){
        std::string overlaypath=srcpath+"/"+layer+"/diff";
        if(!boost::filesystem::exists(overlaypath)){
            std::cerr<<"overlaypath is not exist"<<std::endl;
            continue;
        }
        std::string file_paths;
        boost::filesystem::path dir_path(overlaypath);
        for (boost::filesystem::recursive_directory_iterator it(dir_path), end; it != end; ++it) {
            if (boost::filesystem::is_regular_file(*it)) {
                file_paths=it->path().string();
                auto filedigest=Fromfile(file_paths);
                std::string key=file_paths.substr(overlaypath.length());
                std::replace(key.begin(), key.end(), '\\', '/');
                Pathlist pathlist;
                pathlist.path=key;
                pathlist.value=filedigest->Encoded();
                // checks->Validation[key]=filedigest->Encoded();
                checks->SHA256.push_back(pathlist);
            }
        }
    }
    std::string checkjson=marshal<Check>(*checks);
    std::ofstream file(checkpath,std::ios::trunc);
    if(!file.is_open()){
        std::cerr<<"Failed to open file: "<<checkpath<<std::endl;
        return;
    }
    file<<checkjson;
    file.close();
    std::string target=srcpath+"/"+layers.back()+"/diff/check.json";
    try{
        boost::filesystem::path src_path(checkpath);
        boost::filesystem::path target_path(target);
        Copy_file(src_path, target_path);
        // 删除原文件
        boost::filesystem::remove_all(src_path);
    } catch (const boost::filesystem::filesystem_error& e){
        std::cerr<<"Failed to copy file: "<<e.what()<<std::endl;
    }
}
/**
 * @brief 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
 * 
 * @return std::shared_ptr<ImageSource_interface> 
 */
std::shared_ptr<ImageSource_interface> containerImageRef::NewImageSource(std::shared_ptr<SystemContext>sys,bool check){
    // 1. 得到整个镜像所有层的ID
    auto manifestType=this->preferredManifestType;
    std::vector<std::string>layers;
    if(this->fromImageID!=""&&this->fromImageName!="scratch"){
        layers.push_back(this->fromImageID);//当FROM后面为scratch时，fromImageID为空
    }
    // layers.push_back(this->fromImageID);
    layers.push_back(this->layerID);
    auto destpath=MkdirTemp(getDefaultTmpDir(),"buildah");

    // 2. 构建整个镜像的config和manifest(默认的)
    std::shared_ptr<v1::Image> oimage;
    std::shared_ptr<Manifest> omanifest;
    std::tie(oimage,omanifest)=createConfigsAndManifests();
    auto srcpath=this->store->GetlayerStoragePath();
    
    std::map<Digest, blobLayerInfo> blobLayers;
    // std::vector<std::shared_ptr<Layer>> Layers;
    
    auto s=std::dynamic_pointer_cast<Store>(this->store);
    auto layerstore=std::dynamic_pointer_cast<layerStore>(s->layer_store_use_getters);
    auto now = std::chrono::system_clock::now();
    if(check){
        AddcCheck(layers,srcpath,destpath);
    }
    for(auto layer:layers){
        // 3. 将原目录下diff文件夹下的内容复制到目的缓存目录下，如果目的目录不存在则新建目录
        //  int64_t Copy_directory(const fs::path& source, const fs::path& destination)复制目录的函数已写好，并且返回数据大小
        // auto size=Copy_directory(tarlayerpath,destpath);
        // auto  l=std::make_shared<Layer>();//TODO
        auto l=layerstore->lookup(layer);
        if(l==nullptr){
            std::cerr<<"layer not found"<<std::endl;
            continue;
        }
        auto omediaType=MediaTypeImageLayer;//TODO
        omediaType=computeLayerMIMEType(layer,this->compression);
        auto noCompression=compression::Uncompressed;//TODO
        auto diffOptions=std::make_shared<DiffOptions>();
        diffOptions->Compression->value=noCompression;

        auto tarlayerpath=srcpath+"/"+layer+"/diff";//ocerlay下文件diff缓存
        // 4. 将缓存目的目录下的内容打包成tar文件，返回tar文件流
        auto tarfilepath=destpath+"/layer.tar";
        std::shared_ptr<Digest> tarfile;
        int tarsize;
        std::tie(tarfile,tarsize)=newTarDigester("file",tarfilepath,tarlayerpath);//TODO
        // 5. 计算tar文件的sha256值，并且重命名tar文件
        auto tardigest=tarfile->Encoded();//tar包的sha256
        auto finalBlobName=destpath+"/"+tardigest;//TODO
        try {
            if(boost::filesystem::exists(srcpath+"/"+tardigest)){//文件存在，不重命名
                fs::remove_all(srcpath+"/"+layer);
                layerstore->deleteLayer(tardigest);//删除存在的记录，后面添加最新的记录

            }else{
                boost::filesystem::path src_path(srcpath);
                boost::filesystem::path layer_path(layer);
                boost::filesystem::path target_path(tardigest);
                // 复制文件到目标路径
                // boost::filesystem::copy(src_path / layer_path, src_path / target_path);
                try{
                    boost::filesystem::rename(src_path / layer_path, src_path / target_path);
                }catch(const boost::filesystem::filesystem_error& e){
                    Copy_directory(src_path / layer_path, src_path / target_path);
                    // 删除原文件
                    boost::filesystem::remove_all(src_path / layer_path);
                }
                // Copy_directory(src_path / layer_path, src_path / target_path);
                // 删除原文件
                // boost::filesystem::remove_all(src_path / layer_path);
                // boost::filesystem::rename(src_path / layer_path, src_path / target_path);
                // boost::filesystem::rename(srcpath+"/"+layer, srcpath+"/"+tardigest);//重命名overlay中的文件
            }
            boost::filesystem::rename(tarfilepath, finalBlobName);
            std::cout << "File renamed tarfle successfully: " << tardigest<< std::endl;
        } catch (const boost::filesystem::filesystem_error& e) {
            std::cerr << "Error renaming file: " << e.what() << std::endl;
        }
        l->ID=tardigest;
        l->Names=this->names;
        l->Parent=this->parent;
        l->uncompressedSize=tarsize;
        l->Created=now;
        Descriptor olayerDescriptor;
        olayerDescriptor.MediaType=omediaType;
        olayerDescriptor.Digests=*tarfile;
        olayerDescriptor.Size=tarsize;
        omanifest->Layers.push_back(olayerDescriptor);
        oimage->rootFS.diffIDs.push_back(tarfile->String());

        blobLayers[tardigest].ID=tardigest;
        blobLayers[tardigest].Size=tarsize;
        // Layers.push_back(l);

    }

    // 6. 组织历史记录history，构造appendHistory函数
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);//调试可见
    auto comment=this->historyComment;
    comment+="FROM "+this->fromImageName;
    History onews;
    onews.created=now;
    onews.createdBy=this->createdBy;
    onews.author=oimage->author;
    onews.comment=comment;
    onews.emptyLayer=this->emptyLayer;
    oimage->history.push_back(onews);
    //更新layerstore
    // for(auto layer:Layers){//TODO
    //     layer->Created=now;
    // }
    //删除原有的layer记录
    // layerstore->layers.insert(layerstore->layers.end(),Layers.begin(),Layers.end());
    if(!layerstore->savelayer()){//更新overlay中的layer.json
        std::cerr<<"save layer error"<<std::endl;
    }
    // 7. 将新构建的oci config反序列化为byte，记住marshal函数返回string，需要转化为vector<uint8_t>,函数在
    // std::vector<uint8_t> stringToVector(const std::string& str)
    auto oconfig=marshal<v1::Image>(*oimage);
    auto configdigest=FromString(oconfig);

    // 8. 更改manifest内容，反序列化为byte，记住这个manifest不是最终的manifest，这个时候镜像层没有gzip压缩
    omanifest->Config.Digests=*configdigest;
    omanifest->Config.Size=oconfig.size();
    omanifest->Config.MediaType=MediaTypeImageConfig;
    auto omanifestbytes=marshal<Manifest>(*omanifest);

    auto src=std::make_shared<containerImageSource>();
    src->path=destpath;
    src->ref=std::make_shared<containerImageRef>(*this);
    src->store=this->store;
    src->containerID=this->containerID;
    src->mountLabel=this->mountLabel;
    src->layerID=this->layerID;
    src->names=this->names;
    src->compression=this->compression;
    src->config=stringToVector(oconfig);
    src->configDigest=configdigest;
    src->manifest=stringToVector(omanifestbytes);
    src->manifestType=manifestType;
    src->blobDirectory=this->blobDirectory;
    src->blobLayers=blobLayers;
    return src;
}
/**
 * @brief 创建默认的oci镜像的config和manifest
 * 
 * @return std::tuple<std::shared_ptr<::Image>,std::shared_ptr<Manifest>> 
 */
std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> containerImageRef::createConfigsAndManifests(){
    // 不用管docker格式的
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);//调试可见
    auto oimage=unmarshal<v1::Image>(vectorToString(this->oconfig));

    //调整config和manifest
    oimage.created=now;
    oimage.rootFS.type=TypeLayers;
    oimage.author=BuildAuthor;
    oimage.config.env.push_back("PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin");
    // oimage.config.cmd.push_back("sh");
    if(oimage.platform.OS==""){
        oimage.platform.OS="linux";
    }
    if(oimage.platform.Architecture==""){
        oimage.platform.Architecture="amd64";
    }
    auto omanifest=std::make_shared<Manifest>();
    omanifest->SchemaVersion=2;
    omanifest->MediaType=MediaTypeImageManifest;
    omanifest->Config.MediaType=MediaTypeImageConfig;
    omanifest->Annotations=this->annotations;
    return std::make_tuple(std::make_shared<v1::Image>(oimage),omanifest);
}
/**
 * @brief 只判断oci镜像的mediatype
 * 
 * @param what 
 * @param layerCompression 
 * @return std::string 
 */
std::string computeLayerMIMEType(std::string what,std::shared_ptr<Compression> layerCompression){

    // 如果没有匹配的类型，返回一个空字符串或者错误
    auto omediaType=MediaTypeImageLayer;
    if(layerCompression->value!=compression::Uncompressed){
        if(layerCompression->value==compression::Gzip){
            omediaType=MediaTypeImageLayerGzip;
        }else{
            std::cerr<<"compressing :"+what+"with unknown compressor(?)"<<std::endl;
        }
    }
    
    return omediaType;
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
 * 该函数将返回镜像的所有层信息，该信息包括层的ID、Size、MediaType、CompressionAlgorithm等
 * 该函数将OCI1的LayerInfos()解析为BlobInfo的vector
 * 
 * @return std::vector<BlobInfo> 
 */
std::vector<BlobInfo>  containerImageSource::LayerInfos(){
    // 1. 将manifestBlob解析为OCI1
    auto oci=this->OCI1FromManifest();
    // 2.  OCI1的LayerInfos()解析为BlobInfo的vector
    auto layers=oci->LayerInfos();
    auto blobs = std::vector<BlobInfo>();
    for(auto layer:layers){
        blobs.push_back(*layer.BlobInfo);
    }
    return blobs;
}
/**
 * @brief 返回oci镜像的manifest
 * 
 * @param manifestBlob 
 * @return std::shared_ptr<OCI1> 
 */
std::shared_ptr<OCI1> containerImageSource::OCI1FromManifest(){
    std::string manifeststr=vectorToString(this->manifest);
    auto oci1=unmarshal<OCI1>(manifeststr);
    return std::make_shared<OCI1>(oci1);
}
/**
 * @brief 保存config文件
 * 
 * 该函数将config保存到镜像库中。该函数会将config序列化为json字符串，然后将其写入到
 * 镜像库中。该函数返回true表示写入成功，false表示写入失败
 * 
 * @return true 
 * @return false 
 */
bool containerImageSource::SaveConfig(){
    try{
        std::mutex m;
        std::lock_guard<std::mutex> lock(m);//必须加锁，否则写入内容不一致
        std::string storagepath=this->store->GetImageStoragePath()+"/blobs/sha256/";
        std::string newname;
        std::string configpath=storagepath+"config.json";
        boost::filesystem::ofstream file(configpath,std::ios::binary|std::ios::trunc|std::ios::out);
        file.write(reinterpret_cast<const char*>(config.data()),config.size());
        file.close();
        auto configdigest=Fromfile(configpath);
        newname=storagepath+configdigest->Encoded();
        boost::filesystem::rename(configpath, newname);
        return true;
    }catch(const std::exception& e){
        return false;
    }
}

/**
 * @brief 保存manifest文件
 * 
 * 该函数将manifestbytes（json字符串）保存到镜像库中。该函数会将manifestbytes写入到
 * 镜像库的manifest.json文件中，然后将其重命名。该函数将manifest的sha256值作为
 * 新的文件名。该函数返回包含manifest的sha256值的指针，如果写入失败返回nullptr
 * 
 * @param manifestbytes json字符串
 * @return std::shared_ptr<Digest> 
 */
std::shared_ptr<Digest> containerImageSource::UploadManifest(std::string& manifestbytes){
    try{
        std::mutex m;
        std::lock_guard<std::mutex> lock(m);//必须加锁，否则写入内容不一致
        std::string storagepath=this->store->GetImageStoragePath()+"/blobs/sha256/";
        std::string newname;
        std::string manifestpath=storagepath+"manifest.json";
        boost::filesystem::ofstream file2(manifestpath,std::ios::binary|std::ios::trunc);
        file2.write(reinterpret_cast<const char*>(manifestbytes.data()),manifestbytes.size());
        file2.close();
        auto manifestdigest=Fromfile(manifestpath);
        newname=storagepath+manifestdigest->Encoded();
        boost::filesystem::rename(manifestpath, newname);

        return manifestdigest;
    }catch(const std::exception& e){
        return nullptr;
    }
}
