#include "storage/storage_transport.h"

const std::shared_ptr<StoreTransport_interface> Transport= std::make_shared<storageTransport>();

std::string storageTransport::Name(){
    return "containers-storage";
}