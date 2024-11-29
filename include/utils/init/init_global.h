#if !defined(INIT_GLOBAL_H)
#define INIT_GLOBAL_H
#include "filesys/graphdriver/driver.h"
#include "image/buildah/buildah.h"
#include "filesys/system/cgroups_linux.h"
#include "cobra/command.h"
#include "config/config.h"
#include "config/default_linux.h"
#include "image/types/define/pull.h"
#include "image/digest/algorithm.h"
#include "cmd/build/imagebuilder/evaluator.h"
#include "logrus/bufferpool.h"
#include "logrus/entry.h"
#include "logrus/exported.h"
#include "cmd/root/root.h"
#include "parse/dockerfileparse.h"
#include "regexp/regexp.h"
#include "storage/storage_transport.h"
#include "types/options.h"
#include "image/image_types/v1/descriptor.h"
#include "image/image_types/v1/annotations.h"
#include "image/transports/transports.h"
#include "image/image_types/docker/docker_transport.h"
#include "parse/line_parsers.h"
#include "cmd/build/imagebuilder/builder.h"
#include "cmd/build/imagebuilder/dispatchers.h"
#include "image/types/reference/regexp.h"
#include "storage/overlay/overlay.h"

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
void initialize_global();

#endif // INIT_GLOBAL_H
