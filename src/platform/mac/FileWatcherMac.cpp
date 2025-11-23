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
    // Get the watcher instance
    FileWatcherMac* watcher = static_cast<FileWatcherMac*>(clientCallBackInfo);
    if (!watcher) return;

    char** paths = (char**)eventPaths;

    for (size_t i = 0; i < numEvents; i++) {
        std::cout << "[macOS Event] Path: " << paths[i]
                  << " Flags: " << eventFlags[i] << "\n";

        if (watcher->callback) {
            FileEvent evt;
            evt.path = paths[i];
            evt.flags = eventFlags[i];
            watcher->callback(evt);
        }
    }
}


FileWatcherMac::FileWatcherMac()
    : running(false), streamRef(nullptr), runLoop(nullptr) {}

FileWatcherMac::~FileWatcherMac() {
    stop();
}

void FileWatcherMac::watch(const std::string& path) {
    running = true;

    try {
        absolutePath = std::filesystem::absolute(path).string();
    } catch (...) {
        absolutePath = path;
    }


    CFStringRef cfPath = CFStringCreateWithCString(
        nullptr, absolutePath.c_str(), kCFStringEncodingUTF8);

    CFArrayRef pathsToWatch = CFArrayCreate(
        nullptr, (const void**)&cfPath, 1, nullptr);

    FSEventStreamContext context = {0, this, nullptr, nullptr, nullptr};
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
    dispatchQueue = dispatch_queue_create("com.filemanager.fsevents", DISPATCH_QUEUE_SERIAL);
    FSEventStreamSetDispatchQueue(streamRef, dispatchQueue);
    FSEventStreamStart(streamRef);

    std::cout << "[Watching macOS] " << absolutePath << "\n";

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

void FileWatcherMac::setCallback(EventCallback cb) {
    callback = std::move(cb);
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