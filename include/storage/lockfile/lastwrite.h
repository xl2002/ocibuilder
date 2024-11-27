#if !defined(PKG_LOCKFILE_LASTWRITE_H)
#define PKG_LOCKFILE_LASTWRITE_H
#include <vector>
#include <cstdint>
#include "cobra/error.h"
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/winapi/get_current_process_id.hpp>
#include <atomic>
#include <chrono>
#include <cstring>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;
class lastwrite {
public:
    // Contents of the lock file.
    std::vector<uint8_t> state;
public:
    lastwrite() = default;
    lastwrite(std::vector<uint8_t> s) : state(std::move(s)) {}
};
lastwrite newLastWrite();

#endif