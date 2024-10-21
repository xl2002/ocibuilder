#if !defined(SHORTNAMES_SHORTNAMES_H)
#define SHORTNAMES_SHORTNAMES_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "reference/reference.h"
#include "types/types.h"
#include "digest/digest.h"
#include "define/types.h"

std::vector<std::shared_ptr<Named_interface>> ResolveLocally(std::shared_ptr<SystemContext> ctx,std::string name);
std::tuple<bool,std::shared_ptr<Named_interface>> parseUnnormalizedShortName(std::string input);
std::tuple<bool,bool,std::shared_ptr<Named_interface>,std::string,std::shared_ptr<Digest>> splitUserInput(std::shared_ptr<Named_interface> named);
#endif // SHORTNAMES_SHORTNAMES_H)