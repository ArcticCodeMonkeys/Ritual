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

#endif // TYPES_H