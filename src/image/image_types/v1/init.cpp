#include "image/image_types/v1/annotations.h"
std::string AnnotationBaseImageDigest;
std::string AnnotationBaseImageName;
/**
 * @brief 初始化OCI v1规范的注解常量
 * @details 设置基础镜像摘要和名称的标准注解键值
 */
void v1_init(){
    // AnnotationBaseImageDigest is the annotation key for the digest of the image's base image.
	AnnotationBaseImageDigest = "org.opencontainers.image.base.digest";

	// AnnotationBaseImageName is the annotation key for the image reference of the image's base image.
	AnnotationBaseImageName = "org.opencontainers.image.base.name";
}
