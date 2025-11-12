#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <variant>
#include <memory>
#include <iostream>
using namespace std;

// Basic, non-recursive definitions
struct Action {
    string type;
    vector<string> parameters;
};

struct Condition {
    string lhs = "";
    string op = "";
    string rhs = "";
};

// Forward-declare Command so Conditional can reference it
struct Command;

// A conditional groups a condition with a set of commands
struct Conditional {
    Condition condition;
    vector<Command> commands;
    vector<Command> elseCommands;
};

// A command is either an action or a pointer to a conditional (to break recursive value types)
struct Command {
    variant<Action, shared_ptr<Conditional>> command;
};

struct Ritual {
    string trigger;
    vector<Command> commands;
};

double timeToSeconds(const string& timeStr) {
    // timeStr will be a number followed by a unit, e.g., "10s", "5m", "2h", "7d", "1w"
    if (timeStr.empty()) return 0.0;
    char unit = timeStr.back();
    double value = stod(timeStr.substr(0, timeStr.size() - 1));
    switch (unit) {
        case 's': return value;
        case 'm': return value * 60.0;
        case 'h': return value * 3600.0;
        case 'd': return value * 86400.0;
        case 'w': return value * 604800.0;
        default: return value; // Assume seconds if no valid unit
    }
}

#endif // TYPES_H