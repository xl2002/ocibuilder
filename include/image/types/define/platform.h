#if !defined(DEFINE_PLATFORM_H)
#define DEFINE_PLATFORM_H
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
