#if !defined(SYNC_ONCE_H)
#define SYNC_ONCE_H
#include <mutex>

struct once
{
    uint32_t done;
    std::mutex m;
};

#endif // SYNC_ONCE_H