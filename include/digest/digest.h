#if !defined(GITHUB_DIGEST_H)
#define GITHUB_DIGEST_H
#include <string>
#include <vector>
using std::string;
using std::vector;

class Digest{
    public:
    string digest;
    Digest()=default;
    Digest(string d):digest(d){};
};

#endif // GITHUB_DIGEST_H
