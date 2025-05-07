#include "image/buildah/buildah.h"

std::string Builder::Mount(std::string label){
    auto mountpoint=this->store->GetlayerStoragePath()+Separator+this->container->LayerID;
    this->MountLabel=mountpoint;
    return mountpoint;
}