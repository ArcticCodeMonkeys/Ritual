

#ifndef HELPER_H
#define HELPER_H
#include <string>

static inline std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return std::string();
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

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

static inline std::string getDir(const Ritual& ritual) {
    // Extract directory from ritual trigger string
    size_t pos = ritual.trigger.find("in");
    if (pos == std::string::npos) return ".";
    size_t dirStart = pos + strlen("in");
    size_t dirEnd = ritual.trigger.find_first_of(" \n", dirStart);
    std::string dir = ritual.trigger.substr(dirStart, dirEnd - dirStart);
    return trim(dir);
}
#endif // HELPER_H