#if !defined(IMAGE_TYPES_SIGNATURE_POLICY_CONFIG_H)
#define IMAGE_TYPES_SIGNATURE_POLICY_CONFIG_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <chrono>
#include <iostream>
#include "image/types/define/types.h"
#include "storage/storage/storage.h"
#include "image/types/types.h"
#include "image/types/signature/policy_types.h"


std::shared_ptr<Policy> DefaultPolicy(std::shared_ptr<SystemContext> sys);
std::string defaultPolicyPath(std::shared_ptr<SystemContext> sys);
std::string defaultPolicyPathWithHomeDir(std::shared_ptr<SystemContext> sys,std::string homeDir);
std::shared_ptr<Policy> NewPolicyFromFile(std::string fileName);

#endif // SIGNATURE_POLICY_CONFIG_H)
