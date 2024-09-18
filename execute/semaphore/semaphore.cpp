#include "semaphore/semaphore.h"

shared_ptr<Weighted> NewWeighted(int64_t n)
{
    return std::make_shared<Weighted>(n);
}