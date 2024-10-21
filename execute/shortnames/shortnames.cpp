#include "shortnames/shortnames.h"
#include "cobra/error.h"
#include "sysregistriesv2/shortnames.h"
std::vector<std::shared_ptr<Named_interface>> ResolveLocally(std::shared_ptr<SystemContext> ctx,std::string name){
    bool isShort;
    std::shared_ptr<Named_interface> shortRef;
    std::tie(isShort,shortRef)=parseUnnormalizedShortName(name);
    if(!isShort){

    }
    std::vector<std::shared_ptr<Named_interface>> candidates;
    if(ctx!=nullptr&& ctx->PodmanOnlyShortNamesIgnoreRegistriesConfAndForceDockerHub){
        
    }
    bool isTagged, isDigested;
    std::shared_ptr<Named_interface> shortNameRepo;
    std::string tag;
    std::shared_ptr<Digest> digest;
    std::tie(isTagged,isDigested,shortNameRepo,tag,digest)=splitUserInput(shortRef);
    std::shared_ptr<Named_interface> nameAlias;
    std::tie(nameAlias,std::ignore)=ResolveShortNameAlias(ctx,shortNameRepo->String());

    return candidates;
}

std::tuple<bool,std::shared_ptr<Named_interface>> parseUnnormalizedShortName(std::string input) {
    std::shared_ptr<Reference_interface> ref;
    try{
        ref=Parse(input);
    }catch(const myerror& e){
        return {false,nullptr};
    }
    auto named=std::dynamic_pointer_cast<Named_interface>(ref);
    if(named==nullptr){
        return {false,nullptr};
    }
    auto r=Domain(named);
    return {true,named};
}

std::tuple<bool,bool,std::shared_ptr<Named_interface>,std::string,std::shared_ptr<Digest>> splitUserInput(std::shared_ptr<Named_interface> named) {
    bool isTagged, isDigested;
    std::shared_ptr<Named_interface> normalized;
    std::string tag;
    std::shared_ptr<Digest> digest;
    
    auto tagged=std::dynamic_pointer_cast<NamedTagged_interface>(named);
    if(tagged==nullptr){
        throw myerror("not a tagged reference");
    }
    isTagged=true;
    tag=tagged->Tag();
    auto digested=std::dynamic_pointer_cast<Digested_interface>(named);
    if(digested!=nullptr){
        isDigested=true;
        digest=std::make_shared<Digest>(digested->Digest());
    }
    normalized=TrimNamed(named);
    return {isTagged,isDigested,normalized,tag,digest};
}