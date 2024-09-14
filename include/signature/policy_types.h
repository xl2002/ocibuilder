#if !defined(SIGNATURE_POLICY_TYPES_H)
#define SIGNATURE_POLICY_TYPES_H
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "signature/policy_eval.h"
class PolicyRequirement_interface;
struct PolicyRequirements{
    std::vector<PolicyRequirement_interface> policyrequirements;
};
struct PolicyTransportScopes{
    std::map<std::string,PolicyRequirements> scopes;
};

struct Policy{
    std::shared_ptr<PolicyRequirements> Default=nullptr;
    std::map<std::string,PolicyTransportScopes> Transports;
};


#endif // SIGNATURE_POLICY_TYPES_H)
