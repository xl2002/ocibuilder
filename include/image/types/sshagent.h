#if !defined(IMAGE_TYPES_SSHAGENT_H)
#define IMAGE_TYPES_SSHAGENT_H
#include <string>
#include <vector>
#include <memory>
#include <map>

class Key {
    public:
    // Define members based on the actual key structure
    std::string key;
};

class Source {
    public:
    std::string socket;
    std::vector<Key> keys; // Assuming `interface{}` can be replaced with a specific key type
};
#endif // SSHAGENT_SSHAGENT_H
