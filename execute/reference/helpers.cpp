#include "reference/helpers.h"

bool IsNameOnly(const std::shared_ptr<Named_interface>& ref) {
    if (std::dynamic_pointer_cast<NamedTagged_interface>(ref)) {
        return false;
    }
    if (std::dynamic_pointer_cast<Canonical_interface>(ref)) {
        return false;
    }
    return true;
}