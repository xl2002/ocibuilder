#if !defined(IMAGE_SHORTNAMES_SHORTNAMES_H)
#define IMAGE_SHORTNAMES_SHORTNAMES_H
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include "image/types/reference/reference.h"
#include "image/types/types.h"
#include "image/digest/digest.h"
#include "image/types/define/types.h"
#include "image/types/reference/normalize.h"
std::vector<std::shared_ptr<Named_interface>> ResolveLocally(std::shared_ptr<SystemContext> ctx,std::string name);
std::tuple<bool,std::shared_ptr<Named_interface>> parseUnnormalizedShortName(std::string input);
std::tuple<bool,bool,std::shared_ptr<Named_interface>,std::string,std::shared_ptr<Digest>> splitUserInput(std::shared_ptr<Named_interface> named);
#endif // SHORTNAMES_SHORTNAMES_H)