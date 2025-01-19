#if !defined(SYNC_ONCE_H)
#define SYNC_ONCE_H
#include <mutex>

class once
{
    public:
    uint32_t done;
    std::mutex m;
};

#endif // SYNC_ONCE_H