#if !defined(IMAGE_TYPES_REFERENCE_REFERENCE_H)
#define IMAGE_TYPES_REFERENCE_REFERENCE_H
#include <string>
#include <vector>
#include "image/digest/digest.h"
#include "cobra/error.h"

using std::string;
using std::vector;

class Reference_interface{
    public:
    virtual ~Reference_interface()=default;
    virtual string String()=0;
};

class Named_interface:public Reference_interface{
    public:
    virtual ~Named_interface()=default;
    virtual string Name()=0;
};

class Canonical_interface:public Named_interface{
    public:
    virtual ~Canonical_interface()=default;
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
class NamedTagged_interface:public Named_interface{
    public:
    virtual ~NamedTagged_interface()=default;
    virtual string Tag()=0;
    // string Name() override;
    // string String() override;
};
class Digested_interface:public Reference_interface{
    public:
    virtual ~Digested_interface()=default;
    virtual Digest Digest()=0;
};
class Tagged_interface:public Reference_interface{
    public:
    virtual ~Tagged_interface()=default;
    virtual string Tag()=0;
    // string String() override;
};
class namedRepository_interface:public Named_interface{
    public:
    virtual ~namedRepository_interface()=default;
    virtual std::string Domain()=0;
    virtual std::string Path()=0;
};
class reference : public Canonical_interface{
public:
    std::string tag;
    std::shared_ptr<Digest> digest;
    std::shared_ptr<namedRepository_interface> namedRepository=nullptr;
    reference()=default;
    reference(std::shared_ptr<namedRepository_interface> repo, const std::string& tagValue, std::shared_ptr<Digest> dig)
        : namedRepository(repo), tag(tagValue), digest(dig) {}

    virtual ~reference() = default;

    std::string String() override {
        return namedRepository->Name() + ":" + tag + "@" + digest->String();
    }

    std::string Name() override {
        return namedRepository->Name();
    }

    Digest Digests() override {
        return *digest;
    }
};
class repository:public namedRepository_interface{
    public:
    std::string domain;
    std::string path;
    ~repository()=default;
    string String() override;
    string Name() override;
    string Domain() override;
    string Path() override;
};

struct taggedReference:public NamedTagged_interface{
    std::shared_ptr<namedRepository_interface> namedRepository=nullptr;
    std::string tag;

    taggedReference()=default;
    taggedReference(std::shared_ptr<namedRepository_interface> repo, const std::string& t)
        : namedRepository(repo), tag(t) {}
    std::string String()override{
        return namedRepository->Name() + ":" + tag;
    }
    string Name() override{
        return namedRepository->Name();
    }
    std::string Tag() override{
        return tag;
    }
};
std::shared_ptr<Canonical_interface> WithDigest(std::shared_ptr<Named_interface> name,std::shared_ptr<Digest> digest);
std::shared_ptr<Reference_interface> Parse(std::string s);
std::string Domain(std::shared_ptr<Named_interface> named);
std::shared_ptr<Named_interface> TrimNamed(std::shared_ptr<Named_interface> ref);
std::tuple<std::string,std::string> SplitHostname(std::shared_ptr<Named_interface> named);
#endif // DOCKER_REFERENCE_H
