#include "define/pull.h"


string PullPolicy::String(){
    // if(value==Pull_Policy::PullIfMissing){
    //     return "missing";
    // }else if(value==Pull_Policy::PullAlways){
    //     return "always";
    // }else if(value==Pull_Policy::PullIfNewer){
    //     return "ifnewer";
    // }else if(value==Pull_Policy::PullNever){
    //     return "nerver";
    // }else{
    //     std::cerr<<"unrecognized policy "<<value<<std::endl;
    //     return "";
    // }
    switch(value) {
            case PullIfMissing:
                return "missing";
            case PullAlways:
                return "always";
            case PullIfNewer:
                return "ifnewer";
            case PullNever:
                return "never";
            default:
                std::cerr << "unrecognized policy " << value << std::endl;
                return "";
        }
}

map<string,Pull_Policy>PolicyMap ={
	{"missing",PullIfMissing},
	{"always", PullAlways},
	{"never",  PullNever},
	{"ifnewer", PullIfNewer}
};