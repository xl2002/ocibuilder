#if !defined(IMAGEBUILDAH_BUILD_H)
#define IMAGEBUILDAH_BUILD_H
#include <string>
#include <vector>
#include <memory>
#include "docker/reference.h"
#include "storage/storage.h"
#include "define/build.h"
using std::string;
using std::vector;

string BuildDockerfiles(Store* store, define_BuildOptions options,vector<string> paths,Canonical* ret_ref);

#endif // IMAGEBUILDAH_BUILD_H
