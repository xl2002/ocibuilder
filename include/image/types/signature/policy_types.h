#if !defined(IMAGE_TYPES_SIGNATURE_POLICY_TYPES_H)
#define IMAGE_TYPES_SIGNATURE_POLICY_TYPES_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "image/types/signature/policy_eval.h"
class PolicyRequirement_interface;
struct PolicyRequirements{
    std::vector<PolicyRequirement_interface> policyrequirements;
};
struct PolicyTransportScopes{
    std::map<std::string,PolicyRequirements> scopes;
};

struct Policy{
    std::shared_ptr<PolicyRequirements> Default=std::make_shared<PolicyRequirements>();
    std::map<std::string,PolicyTransportScopes> Transports;
};


#endif // SIGNATURE_POLICY_TYPES_H)
