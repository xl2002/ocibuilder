#include "filesys/utils/homedir_unix.h"
std::string homedir::Key(){
    return "HOME";
}
std::string homedir::Get(){
    auto homedir=HomeDir();
    return homedir;
}