#if !defined(IMAGE_TYPES_SIGNATURE_POLICY_EVAL_H)
#define IMAGE_TYPES_SIGNATURE_POLICY_EVAL_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "image/types/signature/policy_types.h"
class PolicyRequirement_interface{
    public:
};
class PolicyReferenceMatch_interface{
public:
};
class policyContextState{
    public:
    std::string policycontextstate;
};
class Policy;
class PolicyContext{
    public:
    std::shared_ptr<::Policy> Policy=std::make_shared<::Policy>();
    std::shared_ptr<policyContextState> state=std::make_shared<policyContextState>();

    void Destroy();
};
std::shared_ptr <PolicyContext>NewPolicyContext(std::shared_ptr<Policy> policy);
#endif // SIGNATURE_POLICY_EVAL_H
