#if !defined(IMAGEBUILDAH_BUILD_H)
#define IMAGEBUILDAH_BUILD_H
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "reference/reference.h"
#include "storage/storage.h"
#include "define/build.h"
using std::string;
using std::vector;

string BuildDockerfiles(shared_ptr<store> stores, shared_ptr<define_BuildOptions> options,vector<string> paths,shared_ptr<Canonical_interface> ret_ref);

#endif // IMAGEBUILDAH_BUILD_H
