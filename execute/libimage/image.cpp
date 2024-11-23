#include "libimage/image.h"
#include "cobra/error.h"
#include "platforms/platforms.h"
#include "libimage/normalize.h"
namespace LibImage{
    
    std::shared_ptr<ImageReference_interface> Image::StorageReference(){
        auto ref=Transport->ParseStoreReference(this->runtime->store,"@"+this->ID());
        if(ref==nullptr) return nullptr;
        this->storageReference=ref;
        return ref;
    }

    std::shared_ptr<ImageInspectInfo> Image::inspectInfo(){
        auto ref=this->StorageReference();
        if(ref==nullptr){
            throw myerror("storage reference is null");
        }
        auto img=ref->NewImage(this->runtime->systemContext);
        if(img==nullptr){
            throw myerror("new image is null");
        }
        auto imgData=img->Inspect();
        if(imgData==nullptr){
            throw myerror("image data is null");
        }
        this->cached.partialInspectData=imgData;
        return imgData;

    }
    std::shared_ptr<ImageData> Image::Inspect (std::shared_ptr<InspectOptions> options){

    }
    std::string Image::ID(){
        return this->storageImage->ID;
    }

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
    bool Image::isCorrupted(std::string name){
        return false;
    }
}