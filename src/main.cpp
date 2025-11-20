#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <chrono>
#include "Parser.h"
#include "Executor.h"
#include "Watcher.h"

int main(int argc, char** argv) {
    // Load the desktop watcher ritual
    std::ifstream file("tests/desktop_watcher.RSL");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open tests/desktop_watcher.RSL" << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string ritualData = buffer.str();
    
    std::cout << "Ritual Data Loaded:\n" << ritualData << "\n\n";
    
    Ritual ritual;
    parseRitual(ritualData, ritual);
    printTree(ritual, 0);
    
    // Get the Desktop path
    const char* home = getenv("HOME");
    if (!home) {
        std::cerr << "Error: Could not get HOME directory" << std::endl;
        return 1;
    }
    
    std::filesystem::path desktopPath = std::filesystem::path(home) / "Desktop";
    
    std::cout << "\n=== Starting Desktop Watcher ===" << std::endl;
    std::cout << "Watching: " << desktopPath << std::endl;
    std::cout << "Action: Move new files to ~/Downloads" << std::endl;
    std::cout << "Press Ctrl+C to stop...\n" << std::endl;
    
    // Start watching the Desktop
    Watcher* watcher = beginWatching(desktopPath, ritual);
    
    // Keep the program running
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    // Cleanup (won't be reached unless Ctrl+C)
    delete watcher;

    return 0;
}