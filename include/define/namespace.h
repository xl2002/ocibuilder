#if !defined(DEFINE_NAMESPACE_H)
#define DEFINE_NAMESPACE_H
#include <string>
#include <vector>
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
};

#endif // DEFINE_NAMESPACE_H
