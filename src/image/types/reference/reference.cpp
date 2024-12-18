#include "image/types/reference/reference.h"
#include "image/types/reference/helpers.h"
string canonical::String(){
    return "";
}
string canonical::Name(){
    return "";
}
Digest canonical::Digests(){
    Digest d;
    return d;
}
// string NamedTagged::Tag(){
//     return "";
// }
// string NamedTagged::Name(){
//     return "";
// }
// string NamedTagged::String(){
//     return "";
// }
// string Tagged::String(){
//     return "";
// }
// string Tagged::Tag(){
//     return "";
// }

string named::String(){
    return "";
}
string named::Name(){
    return "";
}

std::shared_ptr<Canonical_interface> WithDigest(std::shared_ptr<Named_interface> name,std::shared_ptr<Digest> digest){

    return nullptr;
}
std::shared_ptr<Reference_interface> getBestReferenceType(std::shared_ptr<reference> ref){
    // std::shared_ptr<Reference_interface> ret;
    if(ref->digest->digest==""){
        auto tagref=std::make_shared<taggedReference>();
        tagref->namedRepository=ref->namedRepository;
        tagref->tag=ref->tag;
        return tagref;
    }
    return ref;
}
std::shared_ptr<Reference_interface> Parse(std::string s){
    auto index1=s.find('/');
    auto index2=s.find(':');
    string domain=s.substr(0,index1);
    string path=s.substr(index1+1,index2-index1-1);
    string tag=s.substr(index2+1);
    auto ref=std::make_shared<reference>();
    ref->tag=tag;
    auto repo=std::make_shared<repository>();
    repo->domain=domain;
    repo->path=path;
    ref->namedRepository=repo;
    auto r=getBestReferenceType(ref);
    if(r==nullptr){
        return nullptr;
    }
    return r;
    // return ref;
}
std::string Domain(std::shared_ptr<Named_interface> named){
    auto r=std::dynamic_pointer_cast<namedRepository_interface>(named);
    if(r){
        return r->Domain();
    }
    return "";
}
std::shared_ptr<Named_interface> TrimNamed(std::shared_ptr<Named_interface> ref){
    std::string domain,path;
    std::tie(domain,path)=SplitHostname(ref);
    auto ret=std::make_shared<repository>();
    ret->domain=domain;
    ret->path=path;
    return ret;
}
std::tuple<std::string,std::string> SplitHostname(std::shared_ptr<Named_interface> named){
    auto r=std::dynamic_pointer_cast<namedRepository_interface>(named);
    if(r){
        return {r->Domain(),r->Path()};
    }
    return {"",""};

}
string repository::String(){
    return Name();
}
string repository::Name(){
    if(domain==""){
        return path;
    }
    return domain+"/"+path;
}
string repository::Domain(){
    return domain;
}
string repository::Path(){
    return path;
}