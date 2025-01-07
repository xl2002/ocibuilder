#if !defined(FILESYS_SYSTEMS_H)
#define FILESYS_SYSTEMS_H
// #include "filesys/systems.h"
#include <string>
#ifdef __linux__
#include <pwd.h>
#include <grp.h>
#include <sys/resource.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <sys/vfs.h> // 或者 <sys/statfs.h> 在Linux/Unix系统上
#include <sys/time.h>
#include <sys/utsname.h>
#else
#include <winsock2.h>
#include <windows.h>
#include <boost/winapi/get_current_process_id.hpp>
#endif
std::string OSAndArch();

#endif // FILESYS_SYSTEMS_H)
