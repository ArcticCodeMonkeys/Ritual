#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Types.h"

// Forward declarations
void executeCommand(const Command& cmd);
void executeAction(const Action& action);
bool evaluate(const Condition& condition);

void performRitual(const Ritual& ritual) {
    for (const auto& cmd : ritual.commands) {
        executeCommand(cmd);
    }
}

void executeCommand(const Command& cmd) {
    if (holds_alternative<Action>(cmd.command)) {
        const Action& action = get<Action>(cmd.command);
        executeAction(action);
    } else if (holds_alternative<shared_ptr<Conditional>>(cmd.command)) {
        const auto& conditionalPtr = get<shared_ptr<Conditional>>(cmd.command);
        if (conditionalPtr && evaluate(conditionalPtr->condition)) {
            for (const auto& subCmd : conditionalPtr->commands) {
                executeCommand(subCmd);
            }
        } else if (conditionalPtr) {
            for (const auto& subCmd : conditionalPtr->elseCommands) {
                executeCommand(subCmd);
            }
        }
    }
}


void executeAction(const Action& action) {
    // Implementation of action execution
}

bool evaluate(const Condition& condition) {
    // Implementation of condition evaluation
    // For now, just a placeholder that always returns true
    return true;
}

#endif // EXECUTOR_H