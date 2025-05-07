#include "filesys/utils/homedir_unix.h"
/**
 * @brief Get the environment variable key for home directory
 * @return String containing the home directory environment variable key ("HOME")
 */
std::string homedir::Key(){
    return "HOME";
}
/**
 * @brief Get the current user's home directory path
 * @return String containing the absolute path to the home directory
 * @note Internally calls HomeDir() function to get the actual path
 */
std::string homedir::Get(){
    auto homedir=HomeDir();
    return homedir;
}
