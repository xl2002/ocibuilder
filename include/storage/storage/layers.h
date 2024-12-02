#if !defined(STORAGE_STORAGE_LAYERS_H)
#define STORAGE_STORAGE_LAYERS_H
#include <string>
#include <vector>
using std::string;
using std::vector;
struct Layer
{
    public:
    string ID;
    vector<string> Names;
    string Parent;
    string Metadata;
    string MountLabel ;
};


#endif // STORAGE_LAYERS_H
