#if !defined(IMAGE_TYPES_REFERENCE_NORMALIZE_H)
#define IMAGE_TYPES_REFERENCE_NORMALIZE_H
#include <regex>
#include "image/types/reference/reference.h"
#include "image/types/reference/regexp.h"
#include "image/types/reference/helpers.h"
class normalizedNamed_interface : public Named_interface{
public:
    virtual std::shared_ptr<Named_interface> Familiar()=0;
};
std::shared_ptr<Named_interface> ParseNormalizedNamed(std::string s);
std::shared_ptr<Named_interface> TagNameOnly(const std::shared_ptr<Named_interface>& ref);
std::shared_ptr<Named_interface> ParseDockerRef(const std::string& ref);
std::shared_ptr<NamedTagged_interface> WithTag(std::shared_ptr<Named_interface> name, const std::string& tag);
#endif // REFERENCE_NORMALIZE_H)
