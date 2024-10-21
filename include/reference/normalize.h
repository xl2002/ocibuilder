#if !defined(REFERENCE_NORMALIZE_H)
#define REFERENCE_NORMALIZE_H

#include "reference/reference.h"

class normalizedNamed_interface : public Named_interface{
public:
    virtual std::shared_ptr<Named_interface> Familiar()=0;
};
std::shared_ptr<Named_interface> ParseNormalizedNamed(std::string s);
#endif // REFERENCE_NORMALIZE_H)
