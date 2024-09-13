#if !defined(DOCKER_REFERENCE_H)
#define DOCKER_REFERENCE_H
#include <string>
#include <vector>
#include "digest/digest.h"
using std::string;
using std::vector;

class Reference_interface{
    public:
    ~Reference_interface()=default;
    virtual string String()=0;
};
class Named_interface:public Reference_interface{
    public:
    ~Named_interface()=default;
    virtual string Name()=0;
};

class Canonical_interface:public Named_interface{
    public:
    ~Canonical_interface()=default;
    virtual Digest Digests()=0;
};
class named:public Named_interface{
    public:
    ~named()=default;
    string String() override;
    string Name() override;
};
class canonical:public Canonical_interface{
    public:
    string String() override;
    string Name() override;
    Digest Digests() override;
};
class NamedTagged:public Named_interface{
    public:
    ~NamedTagged()=default;
    string Tag();
    string Name() override;
    string String() override;
};
class Tagged:public Reference_interface{
    public:
    string Tag();
    string String() override;
};
std::shared_ptr<Canonical_interface> WithDigest(std::shared_ptr<Named_interface> name,std::shared_ptr<Digest> digest);

#endif // DOCKER_REFERENCE_H
