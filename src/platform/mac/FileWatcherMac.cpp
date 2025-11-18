#include "FileWatcherMac.h"
#include <iostream>
#include <filesystem>

static void fsevent_callback(
    ConstFSEventStreamRef streamRef,
    void* clientCallBackInfo,
    size_t numEvents,
    void* eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId eventIds[])
{
    char** paths = (char**)eventPaths;

    for (size_t i = 0; i < numEvents; i++) {
        std::cout << "[macOS Event] Path: " << paths[i]
                  << " Flags: " << eventFlags[i] << "\n";
    }
}

FileWatcherMac::FileWatcherMac()
    : running(false), streamRef(nullptr), runLoop(nullptr) {}

FileWatcherMac::~FileWatcherMac() {
    stop();
}

void FileWatcherMac::watch(const std::string& path) {
    running = true;

    std::string absolutePath = std::filesystem::absolute(path).string();



    CFStringRef cfPath = CFStringCreateWithCString(
        nullptr, absolutePath.c_str(), kCFStringEncodingUTF8);

    CFArrayRef pathsToWatch = CFArrayCreate(
        nullptr, (const void**)&cfPath, 1, nullptr);

    FSEventStreamContext context = {0, nullptr, nullptr, nullptr, nullptr};
    //1. Create the stream : "Subscribe to file system events"
    streamRef = FSEventStreamCreate(
        nullptr,
        &fsevent_callback, //where to send back events
        &context,
        pathsToWatch,
        kFSEventStreamEventIdSinceNow,
        1.0,                   // latency
        kFSEventStreamCreateFlagFileEvents // file-level granularity
    );

    CFRelease(cfPath);
    CFRelease(pathsToWatch);

    if (!streamRef) {
        std::cerr << "Failed to create FSEvent stream.\n";
        return;
    }
    //Get run loop: this thread's event dispatcher 
    runLoop = CFRunLoopGetCurrent();
    //2. Schedule and start the stream
    FSEventStreamScheduleWithRunLoop(streamRef, runLoop, kCFRunLoopDefaultMode);

    //Start Monitoring
    FSEventStreamStart(streamRef);

    std::cout << "[Watching macOS] " << path << "\n";

    // Blocking run loop (Phase 1)
    while (running) {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.5, true);
    }
}

void FileWatcherMac::stop() {
    running = false;

    if (streamRef) {
        //3. Stop and clean up the stream
        FSEventStreamStop(streamRef);
        FSEventStreamInvalidate(streamRef);
        FSEventStreamRelease(streamRef);
        streamRef = nullptr;
    }
}

/*

## Visual Summary
```
┌─────────────────────────────────────────────────────┐
│  Application Thread                                 │
├─────────────────────────────────────────────────────┤
│                                                     │
│  std::string path = "./test_dir";                   │
│         ↓ (convert)                                 │
│  CFStringRef cfPath  ← macOS string format          │
│         ↓                                           │
│  FSEventStreamRef streamRef  ← Event subscription   │
│         ↓ (connect to)                              │
│  CFRunLoopRef runLoop  ← Event dispatcher           │
│         ↓                                           │
│  CFRunLoopRunInMode()  ← Wait for events            │ 
│                                                     │
└─────────────────────────────────────────────────────┘
                    ↓
        [File system changes]
                    ↓
         fsevent_callback() fires

*/