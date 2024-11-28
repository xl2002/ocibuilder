#if !defined(IMAGE_TRANSPORTS_ALLTRANSPORTS_H)
#define IMAGE_TRANSPORTS_ALLTRANSPORTS_H

#include "image/types/types.h"
#include <string>
#include <memory>
#include <tuple>
#include <cobra/error.h>
#include "image/transports/transports.h"

std::shared_ptr<ImageReference_interface> ParseImageName(std::string imgName);
std::shared_ptr<ImageTransport_interface> TransportFromImageName(std::string imgName);
#endif // ALLTRANS_ALLTRANSPORTS_H
