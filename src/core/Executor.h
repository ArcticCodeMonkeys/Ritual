#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "Types.h"
#include <filesystem>
#include <fstream>
#include <sstream>

using namespace std;

// Forward declarations
void executeCommand(const Command& cmd, const filesystem::path& obj);
void executeAction(const Action& action, const filesystem::path& obj);
bool evaluate(const Condition& condition, const filesystem::path& obj);

void performRitual(const Ritual& ritual, const vector<filesystem::path>& objects) {
    for (const auto& obj : objects) {
        for (const auto& cmd : ritual.commands) {
            executeCommand(cmd, obj);
        }
    }
}

void executeCommand(const Command& cmd, const filesystem::path& obj) {
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


void executeAction(const Action& action, const filesystem::path& obj) {
        cout << "Executing action: " << action.type << " on " << obj << endl;
        
        if(action.type == "move") {
            // Get destination path and expand ~
            string destStr = action.parameters[0];
            if (destStr[0] == '~') {
                const char* home = getenv("HOME");
                if (home) {
                    destStr = string(home) + destStr.substr(1);
                }
            }
            
            filesystem::path destPath(destStr);
            
            // If destination is a directory, append the filename
            if (filesystem::is_directory(destPath)) {
                destPath = destPath / obj.filename();
            }
            
            cout << "Moving " << obj << " to " << destPath << endl;
            
            try {
                filesystem::rename(obj, destPath);
                cout << "Successfully moved file!" << endl;
            } catch (const filesystem::filesystem_error& e) {
                cerr << "Error moving file: " << e.what() << endl;
            }
        } else if (action.type == "copy") {
            // Get destination path and expand ~
            string destStr = action.parameters[0];
            if (destStr[0] == '~') {
                const char* home = getenv("HOME");
                if (home) {
                    destStr = string(home) + destStr.substr(1);
                }
            }
            
            filesystem::path destPath(destStr);
            
            // If destination is a directory, append the filename
            if (filesystem::is_directory(destPath)) {
                destPath = destPath / obj.filename();
            }
            
            filesystem::copy(obj, destPath);
        } else if (action.type == "delete") {
            filesystem::remove(obj);
        }

}

bool evaluate(const Condition& condition, const filesystem::path& obj) {
    // Implementation of condition evaluation
    if (condition.lhs == "exists") {
        return filesystem::exists(obj);
    } else if (condition.op == "has") {
        if(condition.lhs == "name") {
            return obj.filename().string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "type") {
            return obj.extension().string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "path") {
            return obj.string().find(condition.rhs) != string::npos;
        } else if (condition.lhs == "file") {
            if (filesystem::is_regular_file(obj)) {
                ifstream file(obj);
                stringstream buffer;
                buffer << file.rdbuf();
                string content = buffer.str();
                return content.find(condition.rhs) != string::npos;
            }
        }
    } else if (condition.op == "==") {
        // Look at lhs to see what we are comparing to
        if(condition.lhs == "name") {
            string rhsStr = condition.rhs;
            rhsStr.erase(remove(rhsStr.begin(), rhsStr.end(), '\"'), rhsStr.end());
            return obj.filename().string() == rhsStr;
        } else if (condition.lhs == "type") {
            string rhsStr = condition.rhs;
            rhsStr.erase(remove(rhsStr.begin(), rhsStr.end(), '*'), rhsStr.end());
            // (ie ".txt")
            return obj.extension().string() == rhsStr;
        }
    } else if (condition.lhs == "size") {
        auto fileSize = filesystem::file_size(obj);
        size_t rhsSize = stoull(condition.rhs);
        if (condition.op == ">") {
            return fileSize > rhsSize;
        } else if (condition.op == "<") {
            return fileSize < rhsSize;
        } else if (condition.op == "==") {
            return fileSize == rhsSize;
        }
    } else if (condition.lhs == "is_dir") {
        return filesystem::is_directory(obj);
    } else if (condition.lhs == "is_file") {
        return filesystem::is_regular_file(obj);
    } else if (condition.lhs == "age") {
        auto ftime = filesystem::last_write_time(obj);
        auto sctp = chrono::time_point_cast<chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + chrono::system_clock::now()
        );
        auto fileTime = chrono::system_clock::to_time_t(sctp);
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        double ageInSeconds = difftime(now, fileTime);
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