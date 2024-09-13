#include "reference/reference.h"
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
string NamedTagged::Tag(){
    return "";
}
string NamedTagged::Name(){
    return "";
}
string NamedTagged::String(){
    return "";
}
string Tagged::String(){
    return "";
}
string Tagged::Tag(){
    return "";
}

string named::String(){
    return "";
}
string named::Name(){
    return "";
}

std::shared_ptr<Canonical_interface> WithDigest(std::shared_ptr<Named_interface> name,std::shared_ptr<Digest> digest){

    return nullptr;
}