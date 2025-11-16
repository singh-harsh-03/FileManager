#pragma once

#include "../interfaces/IFileWatcher.h"
#include <CoreServices/CoreServices.h>
#include <string>
#include <atomic>

class FileWatcherMac : public IFileWatcher {
public:
    FileWatcherMac();
    ~FileWatcherMac() override;

    void watch(const std::string& path) override;
    void stop() override;

private:
    std::atomic<bool> running;
    FSEventStreamRef streamRef;
    CFRunLoopRef runLoop;
};
