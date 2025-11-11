#include <fstream>
#include <sstream>
#include <iostream>
#include "Parser.cpp"
#include "Executor.h"

int main(int argc, char** argv) {
    // Get ritual data from example.RSL

    // use a path relative to the project root where the binary is built
    std::ifstream file("src/core/example2.RSL");
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string ritualData = buffer.str();
    cout << "Ritual Data Loaded:\n" << ritualData << "\n\n";
    Ritual ritual;
    parseRitual(ritualData, ritual);
    printTree(ritual, 0);
    performRitual(ritual);

    return 0;
}