#if !defined(SEMAPHORE_SEMAPHORE_H)
#define SEMAPHORE_SEMAPHORE_H
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>
#include <list>
using std::shared_ptr;        

// 定义一个示例任务类
struct Task {

};
class Weighted{
	int64_t size;
	int64_t weight;
	std::mutex mu;
	std::list<Task> waiters;
};
shared_ptr<Weighted> NewWeighted(int64_t n);
#endif // SEMAPHORE_SEMAPHORE_H