#if !defined(SIGNATURE_POLICY_CONFIG_H)
#define SIGNATURE_POLICY_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "define/types.h"
#include "storage/storage.h"
#include "types/types.h"
#include "signature/policy_types.h"


std::shared_ptr<Policy> DefaultPolicy(std::shared_ptr<SystemContext> sys);
std::string defaultPolicyPath(std::shared_ptr<SystemContext> sys);
std::string defaultPolicyPathWithHomeDir(std::shared_ptr<SystemContext> sys,std::string homeDir);
std::shared_ptr<Policy> NewPolicyFromFile(std::string fileName);

#endif // SIGNATURE_POLICY_CONFIG_H)
