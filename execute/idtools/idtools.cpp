#include "idtools/idtools.h"

/**
 * @brief 从用户映射和组映射创建新的IDMappings实例
 *
 * @param uids 用户映射列表
 * @param gids 组映射列表
 *
 * @return std::shared_ptr<IDMappings> 新创建的IDMappings实例
 */
std::shared_ptr<IDMappings> NewIDMappingsFromMaps(std::vector<IDMap> uids, std::vector<IDMap> gids) {

    auto mappings = std::make_shared<IDMappings>();
    mappings->uids = uids;
    mappings->gids = gids;
    return mappings;
}
