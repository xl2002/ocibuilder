#include "image/libimage/image.h"
#include "utils/common/error.h"
#include "filesys/platforms/platforms.h"
#include "image/libimage/normalize.h"
#include "utils/logger/ProcessSafeLogger.h"
namespace LibImage{
    
    /**
     * @brief 获取镜像的存储引用
     * @return std::shared_ptr<ImageReference_interface> 返回镜像存储引用指针
     * @throws 如果无法获取存储引用则返回nullptr
     */
    std::shared_ptr<ImageReference_interface> Image::StorageReference(){
        if(this->storageReference!=nullptr) return this->storageReference;
        auto ref=Transport->ParseStoreReference(this->runtime->store,"@"+this->ID());
        if(ref==nullptr) return nullptr;
        this->storageReference=ref;
        return ref;
    }

    /**
     * @brief 获取镜像检查信息
     * @return std::shared_ptr<ImageInspectInfo> 返回镜像检查信息指针
     * @throws myerror 如果无法获取存储引用或检查信息
     */
    std::shared_ptr<ImageInspectInfo> Image::inspectInfo(){
        auto ref=this->StorageReference();
        if(ref==nullptr){
            logger->log_error("storage reference is null");
            throw myerror("storage reference is null");
        }
        auto img=ref->NewImage(this->runtime->systemContext);
        if(img==nullptr){
            logger->log_error("new image is null");
            throw myerror("new image is null");
        }
        auto imgData=img->Inspect();
        if(imgData==nullptr){
            logger->log_error("image data is null");
            throw myerror("image data is null");
        }
        this->cached.partialInspectData=imgData;
        return imgData;

    }
    /**
     * @brief 检查镜像详细信息
     * @param options 检查选项指针
     * @return std::shared_ptr<ImageData> 返回镜像数据指针
     */
    std::shared_ptr<ImageData> Image::Inspect (std::shared_ptr<InspectOptions> options){

    }
    /**
     * @brief 获取镜像ID
     * @return std::string 返回镜像ID字符串
     */
    std::string Image::ID(){
        return this->storageImage->ID;
    }

    /**
     * @brief 检查镜像是否匹配指定平台
     * @param os 目标操作系统
     * @param arch 目标架构
     * @param variant 目标变体
     * @return bool 如果平台匹配返回true，否则返回false
     */
    bool Image::matchesPlatform( std::string os, std::string arch, std::string variant){
        if(this->isCorrupted("")){
            return false;
        }
        auto inspectInfo=this->inspectInfo();
        auto platform=ToString(os,arch,variant);
        std::shared_ptr<Platform> expected=PlatForms::Parse(platform);
        if(expected==nullptr){
            return false;
        }
        auto fromImage=PlatForms::Parse(ToString(inspectInfo->Os,inspectInfo->Architecture,inspectInfo->Variant));
        if(fromImage==nullptr){
            return false;
        }
        if(expected->OS==fromImage->OS&&expected->Architecture==fromImage->Architecture&&expected->Variant==fromImage->Variant){
            return true;
        }
        return false;
    }
    /**
     * @brief 检查镜像是否损坏
     * @param name 镜像名称(未使用)
     * @return bool 总是返回false
     */
    bool Image::isCorrupted(std::string name){
        return false;
    }
}
