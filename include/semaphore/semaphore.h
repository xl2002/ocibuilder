#if !defined(SEM_SEMAPHORE_H)
#define SEM_SEMAPHORE_H
#include <string>
#include <vector>
#include <mutex>
#include <list>
#include <memory>
class Weighted{
    public:
    int64_t size=0;
    int64_t cur=0;
    std::mutex mu;
    std::list<std::shared_ptr<void>> waiters;
    Weighted(int64_t n):size(n),cur(0){};
};

std::shared_ptr<Weighted> NewWeighted(int64_t n);

#endif // SEM_SEMAPHORE_H
