#if !defined(DEFINE_PULL_H)
#define DEFINE_PULL_H
#include <string>
#include <map>
#include <iostream>
using std::map;
using std::string;
enum Pull_Policy{
    PullIfMissing,
    PullAlways,
    PullIfNewer,
    PullNever
};
class PullPolicy{
    public:
    Pull_Policy value;
	PullPolicy()=default;
    PullPolicy(Pull_Policy v):value(v){};
    string String();
};

extern map<string,Pull_Policy>PolicyMap ;


#endif // DEFINE_
