#if !defined(CMD_BUILD_IMAGEBUILDHAH_BUILD_H)
#define CMD_BUILD_IMAGEBUILDHAH_BUILD_H
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include "image/types/reference/reference.h"
#include "storage/storage/storage.h"
#include "image/types/define/build.h"
#include "utils/common/go/string.h"
#include "utils/common/go/file.h"
#include "utils/common/semaphore.h"
#include "utils/common/group.h"
using std::string;
using std::vector;

string BuildDockerfiles(shared_ptr<Store> stores, shared_ptr<define_BuildOptions> options,vector<string> paths,shared_ptr<Canonical_interface> ret_ref);

#endif // IMAGEBUILDAH_BUILD_H
