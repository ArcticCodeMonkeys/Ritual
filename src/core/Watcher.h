#ifndef WATCHER_H
#define WATCHER_H

#include "Types.h"
#include "Helpers.h"
#include "Executor.h"
#include <filesystem>
#include <efsw/efsw.hpp>
using namespace std;

// Forward declaration
void checkTrigger(const filesystem::path& path, efsw::Action action, const Ritual& ritual);

class Watcher : public efsw::FileWatchListener {
    public:
    efsw::FileWatcher fileWatcher;
    std::unordered_map<efsw::WatchID, filesystem::path> watchMap;

    function<void(const filesystem::path&, efsw::Action)> onEvent;

    void watchDirectory(const filesystem::path& dir, bool recursive = true) {
        efsw::WatchID watchID = fileWatcher.addWatch(dir.string(), this, recursive);
        watchMap[watchID] = dir;
    }

    void start() {
        fileWatcher.watch();
    }

    void handleFileAction( efsw::WatchID watchid,
                           const std::string& dir,
                           const std::string& filename,
                           efsw::Action action,
                           std::string oldFilename = "" ) override {
        filesystem::path fullPath = filesystem::path(dir) / filename;
        if(onEvent) {
            onEvent(fullPath, action);
        }
    }



};


Watcher* beginWatching(const filesystem::path& dir, const Ritual& ritual) {
    Watcher* watcher = new Watcher();
    watcher->onEvent = [ritual](const filesystem::path& path, efsw::Action action) {
        checkTrigger(path, action, ritual);
    };
    watcher->watchDirectory(dir);
    watcher->start();
    
    cout << "Watcher started and monitoring..." << endl;
    
    return watcher;
}

void checkTrigger(const filesystem::path& path, efsw::Action action, const Ritual& ritual) {
    // Simple trigger check based on action type
    string actionType;
    switch(action) {
        case efsw::Actions::Add:
            actionType = "newFile";
            break;
        case efsw::Actions::Delete:
            actionType = "deleteFile";
            break;
        case efsw::Actions::Modified:
            actionType = "modifyFile";
            break;
        case efsw::Actions::Moved:
            actionType = "moveFile";
            break;
        default:
            actionType = "unknown";
    }

    cout << "File event detected: " << actionType << " for " << path << endl;
    cout << "Ritual trigger: " << ritual.trigger << endl;

    if(ritual.trigger.find(actionType) != string::npos) {
        cout << "Trigger matched! Executing ritual..." << endl;
        performRitual(ritual, {path});
    } else {
        cout << "Trigger did not match." << endl;
    }
}

#endif // WATCHER_H