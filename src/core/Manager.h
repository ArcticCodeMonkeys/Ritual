#include "Types.h"
#include "Watcher.h"
#include "Scheduler.h"

void castRituals(const vector<Ritual>& rituals) {
    for (const auto& ritual : rituals) {
        // Determine trigger type and schedule/watch accordingly
        if (ritual.trigger.find("every") != string::npos) {
            scheduleRitual(ritual);
        } else if (ritual.trigger.find("at") != string::npos) {
            scheduleRitualAtTime(ritual);
        } else {
            // Extract directory from trigger for file-based rituals
            string dir = getDir(ritual);
            beginWatching(dir, ritual); // Watch the extracted directory for file events
        }
    }
}