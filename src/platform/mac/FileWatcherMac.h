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
    void setFilter(EventType mask) override;

    EventCallback callback;
    std::string absolutePath;
    EventType filterMask{EventType::All};

    // helpers
    bool createAndStartStream();
    bool flagsMatchMask(uint32_t fseFlags) const;
    static void printFlags(uint32_t flags);

private:
    static void s_fsevents_callback(
        ConstFSEventStreamRef streamRef,
        void* clientCallBackInfo,
        size_t numEvents,
        void* eventPaths,
        const FSEventStreamEventFlags eventFlags[],
        const FSEventStreamEventId eventIds[]);

private:
    std::atomic<bool> running{false};
    FSEventStreamRef streamRef{nullptr};
    dispatch_queue_t dispatchQueue{nullptr};
};

/*
===================== Class Diagram =============================

              +---------------------------+
              |       IFileWatcher        |
              +---------------------------+
              | + watch(path)            |
              | + stop()                 |
              | + setCallback(cb)        |
              +---------------------------+

                       ▲
                       |
                       |
              +---------------------------+
              |       FileWatcherMac      |
              +---------------------------+
              | - absolutePath : string   |
              | - running : atomic<bool>  |
              | - filterMask : EventType  |
              | - callback : EventCallback|
              | - streamRef : FSEvent...  |
              | - dispatchQueue           |
              +---------------------------+
              | + watch(path)            |
              | + stop()                 |
              | + setFilter(mask)        |
              | + setCallback(cb)        |
              | + createAndStartStream() |
              | + flagsMatchMask(f)      |
              +---------------------------+

Flow:
IFileWatcher → FileWatcherMac → macOS FSEvents → Callback → User

=================================================================
*/
