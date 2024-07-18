#if !defined(PARSE_PARSE_H)
#define PARSE_PARSE_H
#include <string>
#include "define/pull.h"
#include "cobra/command.h"
#include "define/types.h"
PullPolicy PullPolicyFromOptions(Command* c);
SystemContext* SystemContextFromOptions(Command* c);
#endif // PARSE_PARSE_H
