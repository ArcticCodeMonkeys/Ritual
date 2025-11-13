#include "Types.h"
#include "Helpers.h"
#include "Executor.h"
#include <filesystem>
#include <efsw/efsw.hpp>
using namespace std;



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


void beginWatching(const filesystem::path& dir, Ritual ritual) {
    Watcher watcher;
    watcher.onEvent = [&](const filesystem::path& path, efsw::Action action) {
        checkTrigger(path, action, ritual);
    };
    watcher.watchDirectory(dir);
    watcher.start();
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

    if(ritual.trigger.find(actionType) != string::npos) {
        performRitual(ritual, {path});
    }
}