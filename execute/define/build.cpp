#include "define/build.h"
const string
	tarExt  = "tar",
	solaris = "solaris",
	windows = "windows",
	darwin  = "darwin",
	freebsd = "freebsd";

string Isolation::String(){
    if(value==IsolationDefault||value==IsolationOCI){
        return "oci";
    }else if(value==IsolationChroot){
        return "chroot";
    }else if(value==IsolationOCIRootless){
        return "rootless";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}

string Compression::String(){
    if(value==Uncompressed){
        return tarExt;
    }else if(value==Bzip2){
        return tarExt+".bz2";
    }else if(value==Gzip){
        return tarExt+".gz";
    }else if(value==Xz){
        return tarExt+".xz";
    }else if(value==Zstd){
        return tarExt+".zst";
    }else{
        std::cerr<<"unrecognized isolation type "<<value<<std::endl;
        return "";
    }
}
string NetworkConfigurationPolicy::String(){
    if(value==NetworkDefault){
        return "NetworkDefault";
    }else if(value==NetworkDisabled){
        return "NetworkDisabled";
    }else if(value==NetworkEnabled){
        return "NetworkEnabled";
    }else{
        std::cerr<<"unknown NetworkConfigurationPolicy "<<value<<std::endl;
        return "";
    }
}
