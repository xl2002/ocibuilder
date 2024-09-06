#include "buildah/buildah.h"
#include "cobra/error.h"
void Builder::Delete(){
    try{
        store->DeleteContainer(ContainerID);
    }catch(const myerror& e){
        throw;
    }
    MountPoint.clear();
    Container.clear();
    ContainerID.clear();
}