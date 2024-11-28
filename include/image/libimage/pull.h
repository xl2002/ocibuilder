#if !defined(IMAGE_LIBIMAGE_PULL_H)
#define IMAGE_LIBIMAGE_PULL_H
#include "image/libimage/copier.h"
struct PullOptions : public CopyOptions{
    bool AllTags = false;
};

#endif // LIBIMAGE_PULL_H)