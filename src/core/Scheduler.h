#include "Types.h"
#include "Helpers.h"


void scheduleRitual(const Ritual& ritual) {
    // Extract interval from trigger string
    size_t everyPos = ritual.trigger.find("every");
    if (everyPos == std::string::npos) return;
    size_t intervalStart = everyPos + strlen("every");
    size_t intervalEnd = ritual.trigger.find_first_of(" \n", intervalStart);
    std::string intervalStr = ritual.trigger.substr(intervalStart, intervalEnd - intervalStart);
    double intervalSeconds = timeToSeconds(trim(intervalStr));
}
void scheduleRitualAtTime(const Ritual& ritual) {
    // Extract time from trigger string
    size_t atPos = ritual.trigger.find("at");
    if (atPos == std::string::npos) return;
    size_t timeStart = atPos + strlen("at");
    size_t timeEnd = ritual.trigger.find_first_of(" \n", timeStart);
    std::string timeStr = ritual.trigger.substr(timeStart, timeEnd - timeStart);
}

