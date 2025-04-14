#include "image/image_types/docker/docker_transport.h"
#include "utils/common/error.h"
#include "utils/common/go/string.h"
/**
 * @brief Get the name of docker transport
 * @return std::string Returns "docker" as the transport name
 */
std::string dockerTransport::Name() {
    return "docker";
}
/**
 * @brief Create a new docker reference
 * @param ref Named interface reference
 * @param unknownDigest Flag indicating if digest is unknown
 * @return std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> 
 *         Tuple containing the new docker reference and error (if any)
 */
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> newReference(std::shared_ptr<Named_interface>ref,bool unknownDigest){
    auto docker=std::make_shared<dockerReference>();
    docker->ref=ref;
    docker->isUnknownDigest=unknownDigest;
    return std::make_tuple(docker, nullptr);
    
    // return std::make_tuple(nullptr, nullptr);
}
/**
 * @brief Create a new docker reference with known digest
 * @param ref Named interface reference
 * @return std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> 
 *         Tuple containing the new docker reference and error (if any)
 */
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> NewReference(std::shared_ptr<Named_interface>ref) {
    return newReference(ref, false);
}
/**
 * @brief Parse a string reference into docker reference object
 * @param refString The reference string to parse (must start with "//")
 * @return std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> 
 *         Tuple containing parsed reference and error (if invalid format)
 * @throws std::exception if parsing fails
 */
std::tuple<std::shared_ptr<ImageReference_interface>,std::shared_ptr<myerror>> ParseReference(std::string refString) {
    
    if(!hasPrefix(refString,"//")){
        return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
    }
    bool unknownDigest = false;
    try{
        auto ref=ParseNormalizedNamed(TrimPrefix(refString,"//"));
        if(!ref){
            return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
        }
        return NewReference(ref);
        // return std::make_tuple(nullptr,nullptr);
    }catch(std::exception &e){
        return std::make_tuple(nullptr,std::make_shared<myerror>("Invalid reference \""+refString+"\""));
    }
}
/**
 * @brief Parse a docker reference string
 * @param reference The reference string to parse
 * @return std::shared_ptr<ImageReference_interface> Parsed docker reference
 * @note Ignores any parsing errors and returns nullptr in case of error
 */
std::shared_ptr<ImageReference_interface> dockerTransport::ParseReference(const std::string &reference) {
    std::shared_ptr<ImageReference_interface> ref;
    std::shared_ptr<myerror> err;
    std::tie(ref,err) =::ParseReference(reference);
    return ref;
}

void dockerTransport::ValidatePolicyConfigurationScope(const std::string &scope) {
    return;
}
/**
 * @brief Get the docker transport interface
 * @return std::shared_ptr<ImageTransport_interface> Shared pointer to docker transport
 */
std::shared_ptr<ImageTransport_interface> dockerReference::Transport() {
    return  docker_Transport;
}
/**
 * @brief Get the string representation of docker reference within transport
 * @return std::string Empty string as docker references don't have transport-specific representation
 */
std::string dockerReference::StringWithinTransport(){
    return  "";
}
/**
 * @brief Get the policy configuration identity for docker reference
 * @return std::string Empty string as docker references don't have policy identity
 */
std::string dockerReference::PolicyConfigurationIdentity(){
    return "";
}
/**
 * @brief Get the policy configuration namespaces for docker reference
 * @return std::vector<std::string> Empty vector as docker references don't have policy namespaces
 */
std::vector<std::string> dockerReference::PolicyConfigurationNamespaces(){
    return std::vector<std::string>();
}
/**
 * @brief Get the underlying docker named reference
 * @return std::shared_ptr<Named_interface> Shared pointer to the named docker reference
 */
std::shared_ptr<Named_interface> dockerReference::DockerReference(){
    return ref;
}
/**
 * @brief Create a new docker image interface
 * @param sys System context for the new image
 * @return std::shared_ptr<Image_interface> Always returns nullptr as not implemented
 */
std::shared_ptr<Image_interface> dockerReference::NewImage(std::shared_ptr<SystemContext>sys){
    return nullptr;
}
/**
 * @brief Create a new docker image source interface
 * @param sys System context for the new image source
 * @param check Flag indicating whether to perform validation checks
 * @return std::shared_ptr<ImageSource_interface> Always returns nullptr as not implemented
 */
std::shared_ptr<ImageSource_interface> dockerReference::NewImageSource(std::shared_ptr<SystemContext>sys,bool check) {
    return nullptr;
}
/**
 * @brief Create a new docker image destination interface
 * @param sys System context for the new image destination
 * @return std::shared_ptr<ImageDestination_interface> Always returns nullptr as not implemented
 */
std::shared_ptr<ImageDestination_interface> dockerReference::NewImageDestination(std::shared_ptr<SystemContext>sys) {
    return nullptr;
}
