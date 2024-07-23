#include "pkg/idtools/idtools.h"

std::shared_ptr<IDMappings> NewIDMappingsFromMaps(std::vector<IDMap> uids, std::vector<IDMap> gids) {

    auto mappings = std::make_shared<IDMappings>();
    mappings->uids = uids;
    mappings->gids = gids;
    return mappings;
}