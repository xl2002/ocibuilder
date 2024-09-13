#include "buildah/buildah.h"

std::string Builder::Architecture(){
    return this->OCIv1->platform->Architecture;
}