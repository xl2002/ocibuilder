#include "imagebuildah/stage_executor.h"
#include "cobra/error.h"
void StageExecutor::Delete(){
    if(builder!=nullptr){
        try{
            builder->Delete();
            builder=nullptr;
        }catch(const myerror& e){
            throw;
        }
    }
}

std::tuple<std::string,std::shared_ptr<canonical>,bool> StageExecutor::Execute(std::string base){



    return std::make_tuple("",std::shared_ptr<canonical>(nullptr),false);
}
