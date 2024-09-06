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