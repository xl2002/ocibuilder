#if !defined(SECURITY_AUTH_H)
#define SECURITY_AUTH_H
#include <string>
#include <memory>
#include "cmd/push/push.h"
#include "utils/common/error.h"
#include <sys/stat.h>
void CheckAuthFile(std::shared_ptr<pushOptions> iopts);

#endif // AUTH_AUTH_H
