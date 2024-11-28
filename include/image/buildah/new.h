#if !defined(IMAGE_BUILDH_NEW_H)
#define IMAGE_BUILDH_NEW_H

#include <vector>
#include <string>

#include "image/buildah/buildah.h"
std::shared_ptr<Builder> newBuilder(std::shared_ptr<Store> store,std::shared_ptr<BuilderOptions> options);

#endif // BUILDH_NEW_H)
