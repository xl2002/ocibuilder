#if !defined(INIT_GLOBAL_H)
#define INIT_GLOBAL_H
#include "graphdriver/driver.h"
#include "buildah/buildah.h"
#include "cgroupv2/cgroups_linux.h"
#include "cobra/command.h"
#include "config/config.h"
#include "config/default_linux.h"
#include "define/pull.h"
#include "digest/algorithm.h"
#include "imagebuilder/evaluator.h"
#include "logrus/bufferpool.h"
#include "logrus/entry.h"
#include "logrus/exported.h"
#include "main/root.h"
#include "parse/dockerfileparse.h"
#include "regexp/regexp.h"
#include "storage/storage_transport.h"
#include "types/options.h"
#include "v1/descriptor.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

void initialize_global();

#endif // INIT_GLOBAL_H
