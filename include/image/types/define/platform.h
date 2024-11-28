#if !defined(IMAGE_TYPES_DEFINE_PLATFORM_H)
#define IMAGE_TYPES_DEFINE_PLATFORM_H
#include <vector>
#include <string>

enum class platformPolicy:int{
    PlatformPolicyDefault,
    PlatformPolicyWarn
};

struct PlatformPolicy{
    platformPolicy policy;
    PlatformPolicy()=default;
    PlatformPolicy(platformPolicy p):policy(p){};
};


#endif // DEFINE_PLATFORM_H)
