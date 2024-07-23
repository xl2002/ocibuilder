#if !defined(IMAGEBUILDAH_BUILD_H)
#define IMAGEBUILDAH_BUILD_H
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "docker/reference.h"
#include "storage/storage.h"
#include "define/build.h"
using std::string;
using std::vector;

string BuildDockerfiles(Store* store, define_BuildOptions options,vector<string> paths,Canonical* ret_ref);
//定义Defer类
class Defer {
public:
    explicit Defer(std::function<void()> func) : func_(std::move(func)) {}
    ~Defer() { func_(); }

private:
    std::function<void()> func_;
};
#endif // IMAGEBUILDAH_BUILD_H
