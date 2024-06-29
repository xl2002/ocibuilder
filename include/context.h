#ifndef CONTEXT_H
#define CONTEXT_H
#include <map>
#include "root.h"

#include "command.h"
// #include "flag.h"

// using namespace std;
class Context{
    public:
        // Rootcmd* rootcmd;
        Command command;
        Flagset* flagset;
        map<string,bool> setflags;
        Context* parent_context;
};


#endif