#pragma once

#include "../interfaces/IFileWatcher.h"
#include <CoreServices/CoreServices.h>
#include <string>
#include <dispatch/dispatch.h>
#include <atomic>

class FileWatcherMac : public IFileWatcher {
public:
    FileWatcherMac();
    ~FileWatcherMac() override;

    void watch(const std::string& path) override;
    void stop() override;
    void setCallback(EventCallback cb) override;

    EventCallback callback; //to store the callback

private:
    std::atomic<bool> running;
    FSEventStreamRef streamRef;
    CFRunLoopRef runLoop;
    dispatch_queue_t dispatchQueue;
    std::string absolutePath;
};
