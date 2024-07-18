#if !defined(DOCKER_REFERENCE_H)
#define DOCKER_REFERENCE_H
#include <string>
#include <vector>
#include "github/digest.h"
using std::string;
using std::vector;

class Reference{
    public:
    ~Reference()=default;
    virtual string String()=0;
};
class Named:public Reference{
    public:
    ~Named()=default;
    virtual string Name()=0;
};
class Canonical:public Named{
    public:
    ~Canonical()=default;
    virtual Digest Digests()=0;
};
class canonical:public Canonical{
    public:
    string String() override;
    string Name() override;
    Digest Digests() override;
};
class NamedTagged:public Named{
    public:
    ~NamedTagged()=default;
    string Tag();
    string String() override;
};
class Tagged:public Reference{
    public:
    string Tag();
    string String() override;
};
// class 

#endif // DOCKER_REFERENCE_H
