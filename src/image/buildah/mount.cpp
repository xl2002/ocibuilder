#include "image/buildah/buildah.h"

/**
 * @brief Mount a container with the specified label
 * 
 * @param label The label identifying the container to mount
 * @return std::string Returns the mount point path as a string
 * 
 * @details This function mounts a container identified by the given label.
 * The mount operation makes the container's filesystem accessible at a mount point.
 * The returned string represents the path where the container is mounted.
 * 
 * @note The caller is responsible for unmounting the container when done.
 * @see Builder::Unmount()
 */
std::string Builder::Mount(std::string label){
    auto mountpoint=this->store->GetlayerStoragePath()+Separator+this->container->LayerID;
    this->MountLabel=mountpoint;
    return mountpoint;
}
