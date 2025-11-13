#ifndef PARSER_H
#define PARSER_H

#include "Types.h"
#include "Helpers.h"
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <sstream>
#include <iostream>

using namespace std;

// Forward declarations
Action parseAction(const string& line);
vector<Command> parseCommands(const vector<string>& lines);
pair<shared_ptr<Conditional>, int> parseConditional(const vector<string>& lines);
Condition parseCondition(const string& line);
void printTree(const Ritual& ritual, int indent);
void printCommand(const Command& cmd, int indent);
void printCondition(const Condition& condition, int indent);

void parseRitual(const string& data, Ritual& ritual) {
    stringstream ss(data);
    vector<string> lines;
    // first line is the trigger
    string triggerLine;
    getline(ss, triggerLine);
    ritual.trigger = trim(triggerLine).substr(strlen("trigger:"));
    while(getline(ss, triggerLine)) {
        lines.push_back(triggerLine);
    }
    ss.clear();
    ss.str("");
    ritual.commands = parseCommands(lines);
}

vector<Command> parseCommands(const vector<string>& lines) {
    vector<Command> commands;
    for (size_t i = 0; i < lines.size(); ++i) {
        const string& line = lines[i];
        string trimmedLine = trim(line);
        
        if (trimmedLine.empty()) continue;
        
        Command cmd;
        if(trimmedLine.rfind("do:", 0) == 0) {
            cmd.command = parseAction(trimmedLine);
            commands.push_back(cmd);
        } else if(trimmedLine.rfind("if:", 0) == 0) {
            // Collect lines for this conditional block
            vector<string> conditionalLines;
            conditionalLines.push_back(trimmedLine);
            
            int depth = 1;
            size_t j = i + 1;
            while (j < lines.size() && depth > 0) {
                string subLine = trim(lines[j]);
                conditionalLines.push_back(lines[j]);
                
                if (subLine.rfind("if:", 0) == 0) {
                    depth++;
                } else if (subLine.rfind("endif", 0) == 0) {
                    depth--;
                }
                j++;
            }
            
            auto [conditional, endifIndex] = parseConditional(conditionalLines);
            cmd.command = conditional;
            commands.push_back(cmd);
            
            // Skip to the line after endif
            i = j - 1;
        }
    }
    return commands;
}

Action parseAction(const string& line) {
    Action action;
    // extract the action type and params after "do:"
    size_t typeStart = line.find("do:") + 3;
    string rest = line.substr(typeStart);
    rest = trim(rest);
    if(rest.empty()) return action;
    string word;
    // Extract words from the rest string, the first is the type and the rest are parameters
    stringstream restSS(rest);
    restSS >> action.type;
    while(restSS >> word) {
        action.parameters.push_back(word);
    }
    return action;
}

pair<shared_ptr<Conditional>, int> parseConditional(const vector<string>& lines) {
    auto conditional = make_shared<Conditional>();
    // First line contains the condition
    conditional->condition = parseCondition(lines[0]);
    
    int elseIndex = -1;
    int endifIndex = -1;
    int depth = 1;
    
    // Find else and endif lines, accounting for nested conditionals
    for (size_t i = 1; i < lines.size(); ++i) {
        string trimmedLine = trim(lines[i]);
        
        if (trimmedLine.rfind("if:", 0) == 0) {
            depth++;
        } else if (trimmedLine.rfind("endif", 0) == 0) {
            depth--;
            if (depth == 0) {
                endifIndex = i;
                break;
            }
        } else if (trimmedLine.rfind("else:", 0) == 0 && depth == 1) {
            elseIndex = i;
        }
    }
    
    // Parse commands for if branch
    vector<string> ifLines;
    size_t endOfIf = (elseIndex != -1) ? elseIndex : endifIndex;
    for (size_t i = 1; i < endOfIf; ++i) {
        ifLines.push_back(lines[i]);
    }
    conditional->commands = parseCommands(ifLines);
    
    // Parse commands for else branch if it exists
    if (elseIndex != -1) {
        vector<string> elseLines;
        for (size_t i = elseIndex + 1; i < (size_t)endifIndex; ++i) {
            elseLines.push_back(lines[i]);
        }
        conditional->elseCommands = parseCommands(elseLines);
    }

    return {conditional, endifIndex};
}

Condition parseCondition(const string& line) {
    Condition condition;
    // Extract the condition string after "if:"
    size_t condStart = line.find(":") + 1;
    string condStr = line.substr(condStart);
    // Simple parsing with format "lhs op rhs"
    stringstream condSS(condStr);
    condSS >> condition.lhs >> condition.op >> condition.rhs;
    return condition;
}

void printTree(const Ritual& ritual, int indent) {
    string indentStr(indent, ' ');
    cout << indentStr << "Ritual Trigger: " << ritual.trigger << endl;
    for(const auto& cmd : ritual.commands) {
        printCommand(cmd, indent + 2);
    }
}

void printCommand(const Command& cmd, int indent) {
    string indentStr(indent, ' ');
    if (holds_alternative<Action>(cmd.command)) {
        const Action& action = get<Action>(cmd.command);
        cout << indentStr << "Action: " << action.type << " Params: ";
        for (const auto& param : action.parameters) {
            cout << param << " ";
        }
        cout << endl;
    } else if (holds_alternative<shared_ptr<Conditional>>(cmd.command)) {
        const auto& conditionalPtr = get<shared_ptr<Conditional>>(cmd.command);
        if (conditionalPtr) {
            const Conditional& conditional = *conditionalPtr;
            cout << indentStr << "If ";
            printCondition(conditional.condition, 0);
            for (const auto& subCmd : conditional.commands) {
                printCommand(subCmd, indent + 4);
            }
            cout << indentStr << "Else:" << endl;
            for (const auto& subCmd : conditional.elseCommands) {
                printCommand(subCmd, indent + 4);
            }
        }
    }
}

void printCondition(const Condition& condition, int indent) {
    string indentStr(indent, ' ');
    cout << indentStr << "Condition: " << condition.lhs << " " << condition.op << " " << condition.rhs << endl;
}

#endif // PARSER_H
