#if !defined(PKG_LOCKFILE_LASTWRITE_H)
#define PKG_LOCKFILE_LASTWRITE_H
#include <vector>
#include <cstdint>
#include "cobra/error.h"
using namespace std;
class lastwrite {
public:
    // Contents of the lock file.
    std::vector<uint8_t> state;
public:
     
};


#endif