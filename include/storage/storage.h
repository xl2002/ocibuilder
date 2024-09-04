#if !defined(STORAGE_STIRAGE_H)
#define STORAGE_STIRAGE_H
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include "idtools/idtools.h"
#include "graphdriver/driver.h"
#include "storage/layers.h"
using std::string;
using std::vector;
using std::map;

struct LayerOptions;
class Store{
    public:
    // RunRoot、GraphRoot、GraphDriverName 和 GraphOptions 检索
	//创建对象时传递给 GetStore() 的设置。
    virtual string RunRoot()=0;
    // virtual string GraphRoot()=0;
    // virtual string ImageStore()=0;
	// virtual bool ransientStore()=0;
	// virtual string GraphDriverName()=0;
	// virtual vector<string> GraphOptions()=0;
	// virtual map<string,string> PullOptions()=0;
	// virtual vector<IDMap> UIDMap()=0;
	// virtual vector<IDMap> GIDMap()=0;
    // //GraphDriver 获取并返回所使用的 graph Driver 对象的句柄
	// virtual std::shared_ptr<Driver> GraphDriver()=0;
    //CreateLayer 在底层存储驱动中创建一个新层，
	//可选地具有指定的 ID（如果没有则分配一个）
	//指定），以指定层（或无层）作为其父层，
	//并带有可选名称。  （可写标志被忽略。）
    // virtual std::shared_ptr<Layer> CreateLayer(string id,string parent,vector<string>names,string mountLabel,bool writeable,LayerOptions* options)=0;
    
};

struct LayerOptions{

};

class store:public Store{
    public:
    string runRoot;
    string graphDriverName;
    vector<string> graphDriverPriority;

    public:
    string RunRoot() override;
};

#endif // STORAGE_STIRAGE_H
