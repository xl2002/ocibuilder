#include "v1/annotations.h"
std::string AnnotationBaseImageDigest;
std::string AnnotationBaseImageName;
void v1_init(){
    // AnnotationBaseImageDigest is the annotation key for the digest of the image's base image.
	AnnotationBaseImageDigest = "org.opencontainers.image.base.digest";

	// AnnotationBaseImageName is the annotation key for the image reference of the image's base image.
	AnnotationBaseImageName = "org.opencontainers.image.base.name";
}