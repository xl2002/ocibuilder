#if !defined(LIBIMAGE_PULL_H)
#define LIBIMAGE_PULL_H
#include "libimage/copier.h"
struct PullOptions : public CopyOptions{
    bool AllTags = false;
};

#endif // LIBIMAGE_PULL_H)