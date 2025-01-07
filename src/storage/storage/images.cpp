#include "storage/storage/images.h"
// #include "image/buildah/image.h"
//recomputeDigests 函数的实现
bool bigDataNameIsManifest(const std::string& name) {
    // 这里 ImageDigestManifestBigDataNamePrefix 是定义为大数据名称的前缀
    const std::string ImageDigestManifestBigDataNamePrefix = "manifest"; 
    return name.find(ImageDigestManifestBigDataNamePrefix) == 0;
}
void storage::Image::recomputeDigests() {
    try {
        // 存储唯一的有效摘要
        std::vector<Digest> validDigests;
        std::set<Digest> uniqueDigests;

        // 处理固定的摘要
        if (!digest->digest.empty()) {
            try {
                digest->Validate();
            }catch (const myerror& e) {
                throw myerror("Validating image digest failed.");
            }
            uniqueDigests.insert(*digest);
            validDigests.push_back(*digest);
        }

        // 处理大数据摘要
        for ( auto& bs : BigDataDigests) {
            auto name=bs.first;
            auto bigDataDigest=bs.second;
            if (!bigDataNameIsManifest(name)) {
                continue;
            }
            try{
                bigDataDigest.Validate();
            }catch (const myerror& e) {
                throw myerror("Validating digest failed for big data item.");
            }
            // 去重
            if (uniqueDigests.find(bigDataDigest) == uniqueDigests.end()) {
                uniqueDigests.insert(bigDataDigest);
                validDigests.push_back(bigDataDigest);
            }
        }

        // 如果固定摘要为空且有效摘要不为空，设置第一个有效摘要
        if (digest->digest.empty() && !Digests.empty()) {
            digest = std::make_shared<Digest>(Digests[0]);
        }

        // 设置计算出的摘要列表
        // Digests = validDigests;
    } catch (const myerror& e) {
        // 捕获并重新抛出 myerror 类型的异常
        throw;
    }
}
// void  storage::index::addManifest(std::shared_ptr<copier> c,std::shared_ptr<copySingleImageResult> single){
//     auto rawSource=c->rawSource;
//     auto dest=c->dest;
//     auto containerimage=std::dynamic_pointer_cast<containerImageSource>(rawSource);
//     if(containerimage==nullptr){
//         std::cerr<<"containerimage is nullptr"<<std::endl;
//         return ;
//     }
//     std::string indexpath=containerimage->store->GetImageStoragePath()+"/index.json";
//     if(!boost::filesystem::exists(indexpath)){
//         std::cout<<"index.json is not exist"<<std::endl;
//         return;
//     }
//     // boost::filesystem::ifstream indexfile(indexpath,std::ios::binary);
//     // 使用 std::ostringstream 将流的内容读取到字符串
//     // std::ostringstream buffer;
//     // buffer << indexfile.rdbuf();  // 读取整个文件内容
//     // std::string fileContent = buffer.str();  // 转换为 std::string
//     // storage::index index;
//     // if(!fileContent.empty()){
//     //     index=unmarshal<storage::index>(fileContent);
//     // }
//     // indexfile.close();
//     auto newmanifest=std::make_shared<storage::manifest>();
//     newmanifest->mediaType=single->manifestMIMEType;
//     newmanifest->digest=single->manifestDigest->String();
//     newmanifest->size=single->manifest.size();
//     auto destimage=std::dynamic_pointer_cast<storageImageDestination>(dest);
//     auto tagref=std::dynamic_pointer_cast<taggedReference>(destimage->imageRef->named);
//     std::string newname=tagref->String();
//     if(destimage!=nullptr&&tagref!=nullptr){
//         newmanifest->annotations["org.opencontainers.image.ref.name"]=newname;
//     }
//     //检测manifest中是否已经有相同的镜像，如果有，删除
//     this->manifests.erase(std::remove_if(this->manifests.begin(), this->manifests.end(), 
//         [newname](const storage::manifest& manifest) {
//             auto it = manifest.annotations.find("org.opencontainers.image.ref.name");
//             return it != manifest.annotations.end() && it->second == newname;
//             }), this->manifests.end());
//     this->manifests.push_back(*newmanifest);
//     this->schemaVersion=2;
//     // index.manifests->annotations["org.opencontainers.image.ref.name"]=tagref->String();
//     // 5. 返回写入新镜像副本的清单
//     // std::string newindexcontent=marshal<storage::index>(index);
//     // std::ofstream newindexfile(indexpath,std::ios::out|std::ios::trunc);
//     // newindexfile<<newindexcontent;
//     // newindexfile.close();
// }

