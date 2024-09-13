#if !defined(SIGNATURE_POLICY_EVAL_H)
#define SIGNATURE_POLICY_EVAL_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "signature/policy_types.h"
struct PolicyRequirement_interface{
    
};
struct PolicyReferenceMatch_interface{

};
struct policyContextState{
    std::string policycontextstate;
};

struct PolicyContext{
    std::shared_ptr<Policy> Policy=nullptr;
    std::shared_ptr<policyContextState> state=nullptr;

    void Destroy();
};
std::shared_ptr <PolicyContext>NewPolicyContext(std::shared_ptr<Policy> policy);
#endif // SIGNATURE_POLICY_EVAL_H)
