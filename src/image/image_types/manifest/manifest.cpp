#include "image/image_types/manifest/manifest.h"

/**
 * @brief 计算manifest blob的摘要
 * @param manifestBlob manifest的二进制数据
 * @return std::shared_ptr<::Digest> 返回计算得到的摘要对象指针
 */
std::shared_ptr<::Digest> manifest::Digest(std::vector<byte> manifestBlob){

    return FromBytes(manifestBlob);
}
