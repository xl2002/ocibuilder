#include "image/buildah/buildah.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
#include "boost/filesystem.hpp"
/**
 * @brief 将destination的内容添加到mountPoint的diff文件夹中
 * 
 * @param destination 
 * @param extract 
 * @param options 
 * @param sources 
 */
void Builder::Add(std::string destination,bool extract,std::shared_ptr<AddAndCopyOptions> options,std::vector<std::string> sources){
//这个函数复杂，简单的按照文件复制处理
    auto mountPoint=this->MountPoint;
    auto contextDir=options->ContextDir;
    auto extractDirectory=boost::filesystem::path(mountPoint+"/diff") / boost::filesystem::path(destination);
    if(!boost::filesystem::exists(extractDirectory)){
        boost::filesystem::create_directories(extractDirectory);
        extractDirectory.make_preferred();
    }
    for(auto source:sources){
        auto srcpath=boost::filesystem::path(source);
        srcpath.make_preferred();
        if(!boost::filesystem::exists(srcpath)){
            std::cout<<"source is not exist"<<std::endl;
            continue;
        }
        if(boost::filesystem::is_directory(srcpath)){
            // fs::path source = "baseimage";
            // fs::path destination = "E:\\BaiduSyncdisk\\OCI\\oci_images\\overlay\\d874630577f501ab2e9884e45ea766ca159c3fabe8e3965017a0326ee7d9b5c6\\diff";
            Copy_directory(srcpath,extractDirectory);
        }else{
            // const boost::filesystem::path& sourcePath = srcpath.path();
            boost::filesystem::path destinationPath = extractDirectory / srcpath.filename();
            boost::filesystem::copy_file(srcpath,destinationPath, boost::filesystem::copy_option::overwrite_if_exists);
        }
    }
}
/**
 * @brief 将更新好的config和manifest保存到镜像库中
 * 
 */
void Builder::Save(std::string name){
    // 获取config对象
    auto config = OCIv1;
    // 序列化config
    auto configjson = marshal(*config);
    
    // 获取存储路径
    std::string storagedir = this->store->GetImageStoragePath() + "/blobs/sha256/";
    
    // 保存config文件
    std::string configPath = storagedir + "config";
    boost::filesystem::ofstream configFile(configPath, std::ios::binary | std::ios::trunc);
    if (!configFile) {
        std::cerr << "Failed to open config file for writing: " << configPath << std::endl;
        return;
    }
    configFile << configjson; // 将config内容写入文件
    configFile.close();
    
    // 计算config文件的sha256哈希值
    auto configDigest = Fromfile(configPath); // 假设Fromfile返回一个包含SHA256值的对象
    std::string newConfigName = storagedir + configDigest->Encoded();
    
    // 如果文件已经存在，则删除
    if (boost::filesystem::exists(newConfigName)) {
        boost::filesystem::remove_all(configPath);
    } else {
        boost::filesystem::rename(configPath, newConfigName);
    }

    // 打印保存成功信息
    // std::cout << "Config saved successfully, new name: " << newConfigName << std::endl;

    // 保存manifest文件
    auto imagestore = this->store->Image(name);
    // this->store->image_store->Save();
    auto olddigest = imagestore->digest->String();  // 获取旧的digest
    auto manifest = imagestore->image_manifest;
    // auto oldconfigdigest = manifest->Config.Digests;
    // auto newconfigdigest = *configDigest;
    // manifest->Config.Digests = newconfigdigest;
    std::string manifestBytes = marshal(*manifest); // 序列化manifest

    // 获取存储路径
    std::string manifestPath = storagedir + "manifest";
    boost::filesystem::ofstream manifestFile(manifestPath, std::ios::binary | std::ios::trunc);
    if (!manifestFile) {
        std::cerr << "Failed to open manifest file for writing: " << manifestPath << std::endl;
        return;
    }
    manifestFile << manifestBytes; // 将manifest内容写入文件
    manifestFile.close();

    // 计算manifest文件的sha256哈希值
    auto manifestDigest = Fromfile(manifestPath); // 假设Fromfile返回一个包含SHA256值的对象
    std::string newManifestName = storagedir + manifestDigest->Encoded();

    // 如果文件已经存在，则覆盖
    if (boost::filesystem::exists(newManifestName)) {
        boost::filesystem::remove_all(newManifestName);  // 删除旧文件
    }
    boost::filesystem::rename(manifestPath, newManifestName);

    // 更新index.json
    std::string indexpath = this->store->GetImageStoragePath() + "/index.json";
    if (!boost::filesystem::exists(indexpath)) {
        std::cout << "index.json does not exist" << std::endl;
        return;
    }

    boost::filesystem::ifstream indexfile(indexpath, std::ios::binary);
    std::ostringstream buffer;
    buffer << indexfile.rdbuf();
    std::string fileContent = buffer.str();
    storage::index index;
    if (!fileContent.empty()) {
        index = unmarshal<storage::index>(fileContent);
    }
    indexfile.close();

    auto newmanifest = std::make_shared<storage::manifest>();
    newmanifest->digest = manifestDigest->String();
    newmanifest->annotations = manifest->Annotations;
    newmanifest->mediaType = manifest->MediaType;

    // 查找相同的镜像并更新（使用旧的digest查找）
    auto m = std::find_if(index.manifests.begin(), index.manifests.end(),
        [olddigest](const storage::manifest& existingManifest) {
            return existingManifest.digest == olddigest;  // 用旧的digest查找
        });

    if (m != index.manifests.end()) {
        // 如果找到相同的镜像，直接替换现有的manifest
        m->digest = newmanifest->digest;  // 更新digest
        m->annotations = newmanifest->annotations;  // 更新annotations
        m->mediaType = newmanifest->mediaType;  // 更新mediaType
        m->historyTags = newmanifest->historyTags;  // 保留历史标签

        // 将历史标签加入新的historyTags
        m->historyTags.push_back(newmanifest->digest);
    } else {
        // 如果没有找到相同的镜像，添加新的manifest
        index.manifests.push_back(*newmanifest);
    }

    // 更新schemaVersion
    index.schemaVersion = 2;

    // 将更新后的index序列化并写回文件
    std::string updatedIndexContent = marshal<storage::index>(index);
    boost::filesystem::ofstream updatedIndexFile(indexpath, std::ios::binary | std::ios::trunc);
    updatedIndexFile << updatedIndexContent;
    updatedIndexFile.close();

}