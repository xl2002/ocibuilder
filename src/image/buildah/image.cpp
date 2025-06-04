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
#include "utils/logger/ProcessSafeLogger.h"
/**
 * @brief 创建容器镜像引用对象
 * @param options 提交选项，包含镜像构建的各种配置参数
 * @return std::shared_ptr<containerImageRef> 容器镜像引用对象指针
 */
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
        logger->log_info("Processing non-directory FromImage with ID: " + this->FromImageID);
        parent=NewDigestFromEncoded(std::make_shared<Algorithm_sha256>(Canonical_sha256),this->FromImageID)->String();
    }

    auto ref=std::make_shared<containerImageRef>();
    ref->fromImageName=this->FromImage;
    ref->fromImageID=container->ImageID;
    if(!this->FromImageDigest.empty()){
        ref->baseImageManifest=this->FromImageDigest.substr(7);
    }
    ref->store=this->store;
    ref->compression=options->Compression;
    ref->names=container->Names;
    ref->name=options->outputimage;
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
/**
 * @brief 获取镜像传输接口
 * @return std::shared_ptr<ImageTransport_interface> 镜像传输接口指针
 */
std::shared_ptr<ImageTransport_interface> containerImageRef::Transport(){
    return nullptr;
}
/**
 * @brief 获取传输范围内的字符串表示
 * @return std::string 传输范围内的标识字符串
 */
std::string containerImageRef::StringWithinTransport(){
    return "";
}
/**
 * @brief 获取策略配置标识
 * @return std::string 策略配置的唯一标识字符串
 */
std::string containerImageRef::PolicyConfigurationIdentity(){
    return "";
}
/**
 * @brief 获取策略配置命名空间
 * @return std::vector<std::string> 策略配置的命名空间列表
 */
std::vector<std::string> containerImageRef::PolicyConfigurationNamespaces(){
    return {};
}
/**
 * @brief 获取 Docker 引用
 * @return std::shared_ptr<Named_interface> Docker 引用指针
 */
std::shared_ptr<Named_interface> containerImageRef::DockerReference(){
    return nullptr;
}
/**
 * @brief 创建新的镜像接口
 * @param sys 系统上下文指针
 * @return std::shared_ptr<Image_interface> 镜像接口指针
 */
std::shared_ptr<Image_interface>  containerImageRef::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}

void AddcCheck(std::string name,std::vector<std::string> layers,std::string srcpath,std::string destpath){
    //TODO
    std::string checkpath=destpath+"/check.json";//中间缓存文件
    auto checks=std::make_shared<Check>();//TODO
    checks->version=8;
    for(auto layer:layers){
        std::string overlaypath=srcpath+"/"+layer+"/diff";
    if(!boost::filesystem::exists(overlaypath)){
        logger->log_error("Overlay path does not exist: " + overlaypath);
        std::cerr<<"overlaypath is not exist"<<std::endl;
        logger->log_info("Skipping non-existent layer: " + layer);
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
                checks->SHA256.push_back(pathlist);
            }
        }
    }
    std::string checkjson=marshal<Check>(*checks);
    std::ofstream file(checkpath,std::ios::trunc);
    if(!file.is_open()){
        logger->log_error("Failed to open check file: " + checkpath);
        std::cerr<<"Failed to open file: "<<checkpath<<std::endl;
        return;
    }
    file<<checkjson;
    file.close();
    std::string layerId=layers.back();
    std::string targets = ":\\/";
    std::replace_if(name.begin(), name.end(),
        [&targets](char c) {
            return targets.find(c) != std::string::npos;
        },'_');
    std::string target=srcpath+"/"+layerId+"/diff/check_"+name+".json";
    try{
        boost::filesystem::path src_path(checkpath);
        boost::filesystem::path target_path(target);
        Copy_file(src_path, target_path);
        boost::filesystem::remove_all(src_path);
        logger->log_info("Successfully copied check file to: " + target);
    } catch (const boost::filesystem::filesystem_error& e){
        std::cerr<<"Failed to copy file: "<<e.what()<<std::endl;
        logger->log_error("Copy operation failed - source: " + checkpath + " destination: " + target + " error: " + e.what());
    }
}
/**
 * @brief 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
 * 
 * @return std::shared_ptr<ImageSource_interface> 
 */
/**
 * @brief 创建新的镜像源接口
 * @param sys 系统上下文指针
 * @param check 是否进行校验检查
 * @return std::shared_ptr<ImageSource_interface> 镜像源接口指针
 * 
 * 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩
 */
/**
 * @brief 创建新的镜像源接口
 * @param sys 系统上下文指针
 * @param check 是否进行校验检查
 * @return std::shared_ptr<ImageSource_interface> 镜像源接口指针
 * 
 * 构建copy层，并构建整个镜像的config和manifest，但是manifest的layer未压缩。
 * 该函数执行以下主要操作：
 * 1. 获取镜像所有层的ID
 * 2. 创建临时工作目录
 * 3. 构建默认的OCI配置和manifest
 * 4. 处理每个镜像层：
 *    - 将层内容复制到临时目录
 *    - 打包为tar文件
 *    - 计算SHA256哈希值
 *    - 重命名文件为哈希值
 * 5. 组织历史记录
 * 6. 更新层存储信息
 * 7. 构建最终的镜像源对象
 * 
 * @note 如果check参数为true，会额外执行校验文件生成和检查
 * @see createConfigsAndManifests() 用于创建默认配置
 * @see computeLayerMIMEType() 用于确定层的MIME类型
 * 
 * 典型用法:
 * @code
 * auto imageSource = imageRef->NewImageSource(nullptr, true);
 * if (!imageSource) {
 *     std::cerr << "Failed to create image source" << std::endl;
 * }
 * @endcode
 */
std::shared_ptr<ImageSource_interface> containerImageRef::NewImageSource(std::shared_ptr<SystemContext>sys,bool check){
    // 1. 得到整个镜像所有层的ID
    auto manifestType=this->preferredManifestType;
    std::vector<std::string>layers;
    if(this->fromImageID!=""&&this->fromImageName!="scratch"){
        auto imagestorePath=this->store->GetImageStoragePath();
        auto Configpath=imagestorePath+"/blobs/sha256/"+this->fromImageID;
    if(!boost::filesystem::exists(Configpath)){
        logger->log_error("Base Image Manifest doesn't exist at path: " + Configpath);
        std::cerr<<"the base Image Manifest isn't exists!"<<std::endl;
        logger->log_error("Failed to load base image: " + this->fromImageName);
        return nullptr;
        }

        boost::filesystem::ifstream Configfile(Configpath,std::ios::binary);
        // 使用 std::ostringstream 将流的内容读取到字符串
        std::ostringstream buffer;
        buffer << Configfile.rdbuf();  // 读取整个文件内容
        std::string fileContent = buffer.str();  // 转换为 std::string
        v1::Image m;
        if(!fileContent.empty()){
            m=unmarshal<v1::Image>(fileContent);
        }
        Configfile.close();
        for(auto& l:m.rootFS.diffIDs){
            layers.push_back(l.substr(7));
        }
    }
    layers.push_back(this->layerID);
    auto destpath=MkdirTemp(getDefaultTmpDir(),"ociBuild");

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
        std::vector<std::string>checklayers{this->layerID};
        AddcCheck(this->name,checklayers,srcpath,destpath);
    }

    for(auto layer:layers){
        // 3. 将原目录下diff文件夹下的内容复制到目的缓存目录下，如果目的目录不存在则新建目录
        //  int64_t Copy_directory(const fs::path& source, const fs::path& destination)复制目录的函数已写好，并且返回数据大小
        // auto size=Copy_directory(tarlayerpath,destpath);
        // auto  l=std::make_shared<Layer>();//TODO
        auto l=layerstore->lookup(layer);
        if(l==nullptr){
            logger->log_error("Layer not found in store: " + layer);
            std::cerr<<"layer not found"<<std::endl;
            logger->log_error("Missing layer in image build process: " + this->name);
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
            if(layer!=tardigest){
                fs::remove_all(srcpath+"/"+layer);
                layerstore->deleteLayer(tardigest);//删除存在的记录，后面添加最新的记录
                logger->log_info("Removed duplicate layer: " + layer);
            }
        }else{
            boost::filesystem::path src_path(srcpath);
            boost::filesystem::path layer_path(layer);
            boost::filesystem::path target_path(tardigest);
            try{
                boost::filesystem::rename(src_path / layer_path, src_path / target_path);
                logger->log_info("Renamed layer from " + layer + " to " + tardigest);
            }catch(const boost::filesystem::filesystem_error& e){
                std::cerr << "cannot renaming file: " << e.what() << std::endl;
                logger->log_error("Rename failed, trying copy - source: " + layer + " dest: " + tardigest + " error: " + e.what());
                Copy_directory(src_path / layer_path, src_path / target_path);
                boost::filesystem::remove_all(src_path / layer_path);
            }
        }
        boost::filesystem::rename(tarfilepath, finalBlobName);
        std::cout<<"Successfully created tar blob: "+tardigest<<std::endl;
        logger->log_info("Successfully created tar blob: " + tardigest);
    } catch (const boost::filesystem::filesystem_error& e) {
        std::cerr << "Error renaming file: " << e.what() << std::endl;
        logger->log_error("Blob creation failed for layer: " + layer + " error: " + e.what());
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
        logger->log_error("Failed to save layer store data for image: " + this->name);
        std::cerr<<"save layer error"<<std::endl;
        logger->log_error("Layer store update failed - image: " + this->name + " layers: " + std::to_string(layers.size()));
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
/**
 * @brief 创建默认的OCI镜像配置和清单
 * @return std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> 
 * 
 * 创建默认的OCI镜像配置和清单，包括设置平台类型、架构、环境变量等基础配置
 */
/**
 * @brief 创建默认的OCI镜像配置和清单
 * @return std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> 
 *         包含镜像配置对象和清单对象的元组
 * 
 * 创建默认的OCI镜像配置和清单，包括以下操作：
 * 1. 初始化镜像创建时间戳
 * 2. 设置根文件系统类型为"layers"
 * 3. 设置默认作者信息
 * 4. 添加默认PATH环境变量
 * 5. 设置默认平台信息(linux/amd64)
 * 6. 初始化清单对象并设置基本属性
 * 
 * 返回的元组包含两个元素：
 * - 第一个元素是v1::Image对象指针，包含镜像配置
 * - 第二个元素是Manifest对象指针，包含清单信息
 * 
 * @note 如果未指定平台信息，将默认使用linux/amd64
 * @see NewImageSource() 使用此函数创建初始配置
 * 
 * 典型用法:
 * @code
 * auto [config, manifest] = imageRef->createConfigsAndManifests();
 * // 修改config和manifest后用于构建镜像
 * @endcode
 */
std::tuple<std::shared_ptr<v1::Image>,std::shared_ptr<Manifest>> containerImageRef::createConfigsAndManifests(){
    // 不用管docker格式的
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);//调试可见
    auto oimage=unmarshal<v1::Image>(vectorToString(this->oconfig));

    //调整config和manifest
    oimage.created=now;
    oimage.rootFS.type=TypeLayers;
    // oimage.author=BuildAuthor;
    if(oimage.platform.OS==""){
        oimage.platform.OS="linux";
        logger->log_info("Using default platform OS: linux");
    }
    if(oimage.platform.Architecture==""){
        oimage.platform.Architecture="amd64";
        logger->log_info("Using default platform architecture: amd64");
    }
    oimage.rootFS.diffIDs.clear();
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
/**
 * @brief 计算层的MIME类型
 * @param what 层标识字符串
 * @param layerCompression 压缩算法指针
 * @return std::string 层的MIME类型字符串
 * 
 * 根据压缩算法确定层的MIME类型，支持未压缩和Gzip压缩类型
 */
/**
 * @brief 计算层的MIME类型
 * @param what 层标识字符串，用于错误消息
 * @param layerCompression 压缩算法指针
 * @return std::string 层的MIME类型字符串
 * 
 * 根据压缩算法确定层的MIME类型，支持以下类型：
 * - 未压缩: MediaTypeImageLayer
 * - Gzip压缩: MediaTypeImageLayerGzip
 * 
 * 该函数执行以下逻辑：
 * 1. 默认返回未压缩的MIME类型
 * 2. 检查压缩算法类型：
 *    - 如果是Gzip压缩，返回Gzip压缩的MIME类型
 *    - 如果是其他未知压缩算法，输出警告信息但仍返回未压缩类型
 * 
 * @note 对于未知压缩算法，会输出警告但不会抛出异常
 * @see NewImageSource() 使用此函数确定层的MIME类型
 * 
 * 典型用法:
 * @code
 * auto mimeType = computeLayerMIMEType("layer1", compression);
 * if (mimeType.empty()) {
 *     std::cerr << "Failed to determine MIME type" << std::endl;
 * }
 * @endcode
 */
std::string computeLayerMIMEType(std::string what,std::shared_ptr<Compression> layerCompression){

    // 如果没有匹配的类型，返回一个空字符串或者错误
    auto omediaType=MediaTypeImageLayer;
    if(layerCompression->value!=compression::Uncompressed){
        if(layerCompression->value==compression::Gzip){
            omediaType=MediaTypeImageLayerGzip;
        }else{
            logger->log_error("compressing :"+what+"with unknown compressor(?)");
            std::cerr<<"compressing :"+what+"with unknown compressor(?)"<<std::endl;
        }
    }
    
    return omediaType;
}

/**
 * @brief 创建新的镜像目标接口
 * @param sys 系统上下文指针
 * @return std::shared_ptr<ImageDestination_interface> 镜像目标接口指针
 * 
 * 当前实现返回nullptr，预留用于未来实现镜像上传功能
 */
/**
 * @brief 创建新的镜像目标接口
 * @param sys 系统上下文指针
 * @return std::shared_ptr<ImageDestination_interface> 镜像目标接口指针
 * 
 * 当前实现返回nullptr，预留用于未来实现镜像上传功能。计划支持以下功能：
 * 1. 创建镜像上传目标
 * 2. 处理镜像层上传
 * 3. 管理上传进度和状态
 * 4. 支持断点续传
 * 5. 处理上传认证和授权
 * 
 * 该接口设计用于：
 * - 将构建好的镜像推送到远程仓库
 * - 支持多种传输协议(HTTP/HTTPS/Docker Registry等)
 * - 处理上传过程中的错误和重试
 * 
 * @note 当前版本尚未实现镜像上传功能
 * @see NewImageSource() 对应的镜像源接口
 * 
 * 典型用法(未来实现):
 * @code
 * auto dest = imageRef->NewImageDestination(nullptr);
 * if (!dest) {
 *     std::cerr << "Failed to create image destination" << std::endl;
 * }
 * // 配置上传参数
 * dest->SetUploadOptions(options);
 * // 执行上传
 * dest->Upload();
 * @endcode
 */
std::shared_ptr<ImageDestination_interface> containerImageRef::NewImageDestination(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
/**
 * @brief 获取镜像引用接口
 * @return std::shared_ptr<ImageReference_interface> 镜像引用接口指针
 * 
 * 返回与当前镜像源关联的镜像引用对象
 */
/**
 * @brief 获取镜像引用接口
 * @return std::shared_ptr<ImageReference_interface> 镜像引用接口指针
 * 
 * 返回与当前镜像源关联的镜像引用对象。该引用对象包含以下信息：
 * - 镜像名称和标签
 * - 存储位置信息
 * - 传输配置
 * 
 * 主要用于：
 * 1. 标识镜像源的身份
 * 2. 提供镜像的存储位置信息
 * 3. 支持镜像的后续操作
 */
std::shared_ptr<ImageReference_interface> containerImageSource::Reference(){
    return this->ref;
}
/**
 * @brief 获取镜像的manifest数据
 * @param instanceDigest 特定manifest实例的摘要指针(可选)
 * @return std::tuple<std::vector<uint8_t>,std::string> 
 *         包含manifest二进制数据和媒体类型的元组
 * 
 * 返回镜像的manifest数据及其媒体类型。如果instanceDigest为nullptr，
 * 则返回默认manifest数据
 */
/**
 * @brief 获取镜像的manifest数据
 * @param instanceDigest 特定manifest实例的摘要指针(可选)
 * @return std::tuple<std::vector<uint8_t>,std::string> 
 *         包含manifest二进制数据和媒体类型的元组
 * 
 * 返回镜像的manifest数据及其媒体类型。如果instanceDigest为nullptr，
 * 则返回默认manifest数据。该函数执行以下操作：
 * 1. 检查instanceDigest参数是否为nullptr
 * 2. 返回存储的manifest二进制数据和媒体类型
 * 
 * @note 当前实现不支持特定实例的manifest获取，总是返回默认manifest
 */
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
/**
 * @brief 获取镜像层信息
 * @return std::vector<BlobInfo> 包含所有层信息的BlobInfo向量
 * 
 * 解析OCI1格式的manifest，提取所有层的信息，包括层ID、大小、媒体类型等。
 * 该函数执行以下操作：
 * 1. 解析manifest数据为OCI1对象
 * 2. 提取所有层的信息
 * 3. 将层信息转换为BlobInfo格式返回
 * 
 * 每个BlobInfo包含以下信息:
 * - Digest: 层的摘要信息
 * - Size: 层的大小(字节)
 * - URLs: 可选的下载URL列表
 * - Annotations: 层的注解信息
 * - MediaType: 层的媒体类型
 * 
 * @note 如果manifest解析失败，将返回空向量
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
/**
 * @brief 从manifest数据创建OCI1对象
 * @return std::shared_ptr<OCI1> OCI1格式的manifest对象指针
 * 
 * 解析manifest数据并转换为OCI1格式的对象，用于后续处理层信息等操作。
 * 该函数执行以下操作:
 * 1. 将二进制manifest数据转换为字符串
 * 2. 使用unmarshal函数解析为OCI1对象
 * 3. 返回封装在shared_ptr中的OCI1对象
 * 
 * @note 如果manifest数据格式无效，将抛出解析异常
 * @see LayerInfos() 使用此函数解析manifest以获取层信息
 * 
 * 典型用法:
 * @code
 * auto oci1 = imageSource->OCI1FromManifest();
 * auto layers = oci1->LayerInfos();
 * @endcode
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
/**
 * @brief 保存镜像配置到存储
 * @return bool 保存成功返回true，失败返回false
 * 
 * 将镜像配置序列化为JSON格式并保存到镜像库的blobs/sha256目录下，
 * 文件以配置内容的SHA256哈希值命名。该函数执行以下操作：
 * 1. 使用互斥锁确保线程安全
 * 2. 将配置数据写入临时文件config.json
 * 3. 计算文件的SHA256哈希值
 * 4. 将文件重命名为哈希值
 * 
 * @note 如果写入或重命名过程中发生异常，将返回false
 * @see UploadManifest() 类似的文件保存机制
 * 
 * 典型错误处理：
 * @code
 * if (!imageSource->SaveConfig()) {
 *     std::cerr << "Failed to save image config" << std::endl;
 * }
 * @endcode
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
 * @brief 上传并保存manifest文件
 * @param manifestbytes manifest的JSON字符串内容
 * @return std::shared_ptr<Digest> 返回manifest的SHA256摘要指针，失败返回nullptr
 * 
 * 将manifest内容写入临时文件，计算其SHA256哈希值后重命名文件，
 * 使用互斥锁确保线程安全。该函数执行以下操作：
 * 1. 使用互斥锁确保线程安全
 * 2. 将manifest数据写入临时文件manifest.json
 * 3. 计算文件的SHA256哈希值
 * 4. 将文件重命名为哈希值
 * 
 * @note 如果写入或重命名过程中发生异常，将返回nullptr
 * @see SaveConfig() 类似的配置文件保存机制
 * 
 * 典型用法:
 * @code
 * auto digest = imageSource->UploadManifest(manifestJson);
 * if (!digest) {
 *     std::cerr << "Failed to upload manifest" << std::endl;
 * }
 * @endcode
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
