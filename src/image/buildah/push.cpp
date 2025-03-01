#include "image/buildah/push.h"
#include "utils/common/error.h"
#include "image/types/blobcache.h"
LookupReferenceFunc cacheLookupReferenceFunc(std::string directory, std::shared_ptr<LayerCompression> compression){
    return [=](std::shared_ptr<ImageReference_interface>ref)->std::shared_ptr<ImageReference_interface>{
        if(directory==""){
            return ref;
        }
        try{
            auto new_ref=NewBlobCache(ref,directory, compression);
            return new_ref;
        }catch(const myerror& e){
            throw myerror("creating blob cache: "+std::string(e.what()));
        }
    };
}
