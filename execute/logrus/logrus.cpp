#include "utils/logger/logrus/logrus.h"

std::string Level::String() {
    try
    {
        auto b=MarshalText();
        return std::string(b.begin(),b.end());
    }
    catch(const myerror& e)
    {
        return "unknown";
    }
}

std::vector<uint8_t> Level::MarshalText(){
    if(lvl==level::TraceLevel){
        return std::vector<uint8_t>({'t', 'r', 'a', 'c', 'e'});
    }
    if(lvl==level::DebugLevel){
        return std::vector<uint8_t>({'d', 'e', 'b', 'u', 'g'});
    }
    if(lvl==level::InfoLevel){
        return std::vector<uint8_t>({'i', 'n', 'f', 'o'});
    }
    if(lvl==level::WarnLevel){
        return std::vector<uint8_t>({'w', 'a', 'r', 'n'});
    }
    if(lvl==level::ErrorLevel){
        return std::vector<uint8_t>({'e', 'r', 'r', 'o', 'r'});
    }
    if(lvl==level::FatalLevel){
        return std::vector<uint8_t>({'f', 'a', 't', 'a', 'l'});
    }
    if(lvl==level::PanicLevel){
        return std::vector<uint8_t>({'p', 'a', 'n', 'i', 'c'});
    }
    throw myerror("not a valid logrus level "+std::to_string((int)lvl));
}
Level ParseLevel(std::string lvl){
    std::transform(lvl.begin(), lvl.end(), lvl.begin(), ::tolower);
    if(lvl=="trace"){
        return Level(level::TraceLevel);
    }
    else if(lvl=="debug"){
        return Level(level::DebugLevel);
    }
    else if(lvl=="info"){
        return Level(level::InfoLevel);
    }
    else if(lvl=="warn"){
        return Level(level::WarnLevel);
    }
    else if(lvl=="error"){
        return Level(level::ErrorLevel);
    }
    else if(lvl=="fatal"){
        return Level(level::FatalLevel);
    }
    else if(lvl=="panic"){
        return Level(level::PanicLevel);
    }else{
        throw myerror("not a valid logrus Level: "+lvl);
    }
}
void Level:: UnmarshalText(std::vector<uint8_t> text){
    try
    {
        auto l=ParseLevel(std::string(text.begin(),text.end()));
        lvl=l.lvl;
    }
    catch(const myerror& e)
    {
        throw;
    }
}

