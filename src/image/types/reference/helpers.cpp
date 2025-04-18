#include "image/types/reference/helpers.h"

/**
 * @brief 检查引用是否仅包含名称
 * 
 * @param ref 要检查的命名接口指针
 * @return bool 如果引用仅包含名称返回true，否则返回false
 */
bool IsNameOnly(const std::shared_ptr<Named_interface>& ref) {
    if (std::dynamic_pointer_cast<NamedTagged_interface>(ref)) {
        return false;
    }
    if (std::dynamic_pointer_cast<Canonical_interface>(ref)) {
        return false;
    }
    return true;
}
