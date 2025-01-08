#include "image/types/signature/policy_eval.h"
std::shared_ptr <PolicyContext>NewPolicyContext(std::shared_ptr<Policy> policy){


    return std::make_shared<PolicyContext>();
}

void PolicyContext::Destroy(){

}