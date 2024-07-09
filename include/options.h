#if !defined(OPTIONS_H)
#define OPTIONS_H
#include <string>
#include <map>
#include <vector>
using std::string;
using std::map;
using std::vector;
class StoreOptions{
    public:
    string GraphRoot;
    string ImageStore;
    string GraphDriverName;
    vector<string> GraphDriverPriority;
    vector<string> GraphDriverOptions;
    map<string,string> PullOptions;
};

StoreOptions DefaultStoreOptions();


#endif // MACRO
