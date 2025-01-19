#if !defined(IMAGE_TYPES_DEFINE_NAMESPACE_H)
#define IMAGE_TYPES_DEFINE_NAMESPACE_H
#include <string>
#include <vector>
#include<algorithm>
using std::vector;
using std::string;
class NamespaceOption{
    public:
    string Name;
    bool Host;
    string Path;
};
class NamespaceOptions{
    public:
    vector<NamespaceOption> val;
    void AddOrReplace(vector<NamespaceOption> options);
};

#endif // DEFINE_NAMESPACE_H
