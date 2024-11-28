#if !defined(FILESYS_SYSTEM_LCOW_UNIX_H)
#define FILESYS_SYSTEM_LCOW_UNIX_H
#include <string>
bool isLCOWSupported() {
    // This should return whether LCOW (Linux Containers on Windows) is supported.
    // You need to implement this according to your actual environment.
    return false;
}
std::string getRuntimeGOOS() {
        // This should return the current OS, for example, "windows" or "linux".
        // You need to implement this according to your actual environment.
        #ifdef _WIN32
        return "windows";
        #elif __linux__
        return "linux";
        #else
        return "unknown";
        #endif
    }
#endif // SYSTEM_LCOW_UNIX_H
