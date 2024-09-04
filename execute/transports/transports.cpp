#include "transports/transports.h"
auto kt=std::make_shared<knownTransports>();
std::shared_ptr<ImageTransport> Get(std::string name) {
    return kt->Get(name);
}
void Delete(std::string name) {
    kt->Remove(name);
}

void Register(std::shared_ptr<ImageTransport> t) {
    kt->Add(t);
}
std::shared_ptr<ImageTransport> knownTransports::Get(std::string name){
    std::lock_guard<std::mutex> lock(mu);
    if(transports.find(name)==transports.end()){
        return nullptr;
    }
    return transports[name];
}
void knownTransports::Remove(std::string k){
    std::lock_guard<std::mutex> lock(mu);
    transports.erase(k);
}
void knownTransports::Add(std::shared_ptr<ImageTransport> t){
    std::lock_guard<std::mutex> lock(mu);
    auto name=t->Name();
    if(transports.find(name)!=transports.end()){
        throw myerror("Duplicate image transport name "+name);
    }
    transports[name]=t;
}