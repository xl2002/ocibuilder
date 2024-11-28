#if !defined(IMAGE_IMAGE_TYPES_V1_MEDIATYPE_H)
#define IMAGE_IMAGE_TYPES_V1_MEDIATYPE_H

#include <string>
const std::string 	
    // MediaTypeDescriptor specifies the media type for a content descriptor.
	MediaTypeDescriptor = "application/vnd.oci.descriptor.v1+json",

	// MediaTypeLayoutHeader specifies the media type for the oci-layout.
	MediaTypeLayoutHeader = "application/vnd.oci.layout.header.v1+json",

	// MediaTypeImageIndex specifies the media type for an image index.
	MediaTypeImageIndex = "application/vnd.oci.image.index.v1+json",

	// MediaTypeImageManifest specifies the media type for an image manifest.
	MediaTypeImageManifest = "application/vnd.oci.image.manifest.v1+json",

	// MediaTypeImageConfig specifies the media type for the image configuration.
	MediaTypeImageConfig = "application/vnd.oci.image.config.v1+json",

	// MediaTypeEmptyJSON specifies the media type for an unused blob containing the value "{}".
	MediaTypeEmptyJSON = "application/vnd.oci.empty.v1+json";

const std::string
    // MediaTypeImageLayer is the media type used for layers referenced by the manifest.
	MediaTypeImageLayer = "application/vnd.oci.image.layer.v1.tar",

	// MediaTypeImageLayerGzip is the media type used for gzipped layers
	// referenced by the manifest.
	MediaTypeImageLayerGzip = "application/vnd.oci.image.layer.v1.tar+gzip",

	// MediaTypeImageLayerZstd is the media type used for zstd compressed
	// layers referenced by the manifest.
	MediaTypeImageLayerZstd = "application/vnd.oci.image.layer.v1.tar+zstd";

#endif // V1_MEDIATYPE_H
