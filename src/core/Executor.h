#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Types.h"
#include <filesystem>
#include <fstream>
#include <sstream>

// Forward declarations
void executeCommand(const Command& cmd);
void executeAction(const Action& action);
bool evaluate(const Condition& condition);

void performRitual(const Ritual& ritual, const vector<std::filesystem::path>& objects) {
    for (const auto& obj : objects) {
        for (const auto& cmd : ritual.commands) {
            executeCommand(cmd, obj);
        }
    }
}

void executeCommand(const Command& cmd, const std::filesystem::path& obj) {
    if (holds_alternative<Action>(cmd.command)) {
        const Action& action = get<Action>(cmd.command);
        executeAction(action, obj);
    } else if (holds_alternative<shared_ptr<Conditional>>(cmd.command)) {
        const auto& conditionalPtr = get<shared_ptr<Conditional>>(cmd.command);
        if (conditionalPtr && evaluate(conditionalPtr->condition, obj)) {
            for (const auto& subCmd : conditionalPtr->commands) {
                executeCommand(subCmd, obj);
            }
        } else if (conditionalPtr) {
            for (const auto& subCmd : conditionalPtr->elseCommands) {
                executeCommand(subCmd, obj);
            }
        }
    }
}


void executeAction(const Action& action, const std::filesystem::path& obj) {
        if(action.type == "move") {
            std::filesystem::rename(obj, action.parameters[0]);
        } else if (action.type == "copy") {
            std::filesystem::copy(obj, action.parameters[0]);
        } else if (action.type == "delete") {
            std::filesystem::remove(obj);
        }

}

bool evaluate(const Condition& condition, const std::filesystem::path& obj) {
    // Implementation of condition evaluation
    if (condition.lhs == "exists") {
        return std::filesystem::exists(obj);
    } else if (condition.op == "has") {
        if(condition.lhs == "name") {
            return obj.filename().string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "type") {
            return obj.extension().string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "path") {
            return obj.string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "file") {
            if (std::filesystem::is_regular_file(obj)) {
                std::ifstream file(obj);
                std::stringstream buffer;
                buffer << file.rdbuf();
                string content = buffer.str();
                return content.find(condition.rhs) != string::npos;
            }
        }
    } else if (condition.op == "==") {
        // Look at lhs to see what we are comparing to
        if(condition.lhs == "name") {
            string rhsStr = condition.rhs;
            rhsStr.erase(std::remove(rhsStr.begin(), rhsStr.end(), '\"'), rhsStr.end());
            return obj.filename().string() == rhsStr;
        } else if (condition.lhs == "type") {
            string rhsStr = condition.rhs;
            rhsStr.erase(std::remove(rhsStr.begin(), rhsStr.end(), '*'), rhsStr.end());
            // (ie ".txt")
            return obj.extension().string() == rhsStr;
        }
    } else if (condition.lhs == "size") {
        auto fileSize = std::filesystem::file_size(obj);
        size_t rhsSize = std::stoull(condition.rhs);
        if (condition.op == ">") {
            return fileSize > rhsSize;
        } else if (condition.op == "<") {
            return fileSize < rhsSize;
        } else if (condition.op == "==") {
            return fileSize == rhsSize;
        }
    } else if (condition.lhs == "is_dir") {
        return std::filesystem::is_directory(obj);
    } else if (condition.lhs == "is_file") {
        return std::filesystem::is_regular_file(obj);
    } else if (condition.lhs == "age") {
        auto ftime = std::filesystem::last_write_time(obj);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now()
        );
        auto fileTime = std::chrono::system_clock::to_time_t(sctp);
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        double ageInSeconds = std::difftime(now, fileTime);
        double rhsAge = timeToSeconds(condition.rhs);
        if (condition.op == ">") {
            return ageInSeconds > rhsAge;
        } else if (condition.op == "<") {
            return ageInSeconds < rhsAge;
        } else if (condition.op == "==") {
            return ageInSeconds == rhsAge;
        }
    }
    return false;
}

#endif // EXECUTOR_H