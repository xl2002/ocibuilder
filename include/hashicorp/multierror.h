#if !defined(HASHICORP_MULTIERROR)
#define HASHICORP_MULTIERROR
#include <future>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include "cobra/error.h"

class Group
{
public:
    std::mutex mutex_;
    std::vector<std::future<void>> futures_;
    std::shared_ptr<myerror> err_=std::make_shared<myerror>();

public:
    void Go(std::function<void()> f);
    std::shared_ptr<myerror> Wait();
};

#endif // HASHICORP_MULTIERROR