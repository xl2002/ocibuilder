/**
 * @file define.cpp
 * @author xiongkang (xiongkang@mail.nwpu.edu.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "define.h"
const string
	tarExt  = "tar",
	solaris = "solaris",
	windows = "windows",
	darwin  = "darwin",
	freebsd = "freebsd";
enum Pull_Policy{
    PullIfMissing,
    PullAlways,
    PullIfNewer,
    PullNever
};
string PullPolicy::String(){
    if(value=Pull_Policy::PullIfMissing){
        return "missing";
    }else if(value==Pull_Policy::PullAlways){
        return "always";
    }else if(value==Pull_Policy::PullIfNewer){
        return "ifnewer";
    }else if(value==Pull_Policy::PullNever){
        return "nerver";
    }else{
        std::cerr<<"unrecognized policy "<<value<<std::endl;
        return "";
    }
}

map<string,Pull_Policy>PolicyMap ={
	{"missing",PullIfMissing},
	{"always", PullAlways},
	{"never",  PullNever},
	{"ifnewer", PullIfNewer}
};
string Isolation::String(){
    if(value==isolation::IsolationDefault||value==isolation::IsolationOCI){
        return "oci";
    }else if(value==isolation::IsolationChroot){
        return "chroot";
    }else if(value==isolation::IsolationOCIRootless){
        return "rootless";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}
enum isolation{
    IsolationDefault,
    IsolationOCI,
    IsolationChroot,
    IsolationOCIRootless
};
string Compression::String(){
    if(value==compression::Uncompressed){
        return tarExt;
    }else if(value==compression::Bzip2){
        return tarExt+".bz2";
    }else if(value==compression::Gzip){
        return tarExt+".gz";
    }else if(value==compression::Xz){
        return tarExt+".xz";
    }else if(value==compression::Zstd){
        return tarExt+".zst";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}
enum compression{
    Uncompressed,
    Bzip2,
    Gzip,
    Xz,
    Zstd
};
string NetworkConfigurationPolicy::String(){
    if(value==networkConfigurationPolicy::NetworkDefault){
        return "NetworkDefault";
    }else if(value==networkConfigurationPolicy::NetworkDisabled){
        return "NetworkDisabled";
    }else if(value==networkConfigurationPolicy::NetworkEnabled){
        return "NetworkEnabled";
    }else{
        std::cerr<<"unknown NetworkConfigurationPolicy "<<value<<std::endl;
        return "";
    }
}
enum networkConfigurationPolicy{
    NetworkDefault,
    NetworkDisabled,
    NetworkEnabled
};