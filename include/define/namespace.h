#if !defined(DEFINE_NAMESPACE_H)
#define DEFINE_NAMESPACE_H
#include <string>
#include <vector>
#include<algorithm>
using std::vector;
using std::string;
struct NamespaceOption{
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
