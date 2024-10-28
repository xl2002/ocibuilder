#if !defined(REFERENCE_NORMALIZE_H)
#define REFERENCE_NORMALIZE_H
#include <regex>
#include "reference/reference.h"
#include "reference/regexp.h"
class normalizedNamed_interface : public Named_interface{
public:
    virtual std::shared_ptr<Named_interface> Familiar()=0;
};
std::shared_ptr<Named_interface> ParseNormalizedNamed(std::string s);
std::shared_ptr<Named_interface> ParseDockerRef(const std::string& ref);
#endif // REFERENCE_NORMALIZE_H)
