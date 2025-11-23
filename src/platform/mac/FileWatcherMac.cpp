#include "FileWatcherMac.h"
#include <iostream>
#include <filesystem>

void FileWatcherMac::setFilter(EventType mask) {
    filterMask = mask;
}


FileWatcherMac::FileWatcherMac() = default;

FileWatcherMac::~FileWatcherMac() {
    stop();
}

void FileWatcherMac::watch(const std::string& path) {
    if (running.load()) {
        std::cout << "[FileWatcherMac] Watcher already running\n";
        return;
    }

    try {
        absolutePath = std::filesystem::weakly_canonical(path).string();
    } catch (...) {
        try {
            absolutePath = std::filesystem::absolute(path).string();
        } catch (...) {
            absolutePath = path;
        }
    }
    running = true;

    if (!createAndStartStream()) {
        std::cerr << "[FileWatcherMac] Failed to create/start stream for: " << absolutePath << "\n";
        running = false;
        return;
    }

    std::cout << "[Watching macOS] " << absolutePath << "\n";

}

void FileWatcherMac::stop() {
    if (!running.exchange(false)) return; // already stopped
    if (streamRef) {
        //3. Stop and clean up the stream
        FSEventStreamStop(streamRef);
        FSEventStreamSetDispatchQueue(streamRef, nullptr);
        FSEventStreamInvalidate(streamRef);
        FSEventStreamRelease(streamRef);
        streamRef = nullptr;
    }
}

bool FileWatcherMac::createAndStartStream() {
    if (streamRef) {
        // already running — stop and recreate
        stop();
    }

    CFStringRef cfPath = CFStringCreateWithCString(nullptr, absolutePath.c_str(), kCFStringEncodingUTF8);
    if (!cfPath) {
        std::cerr << "[FileWatcherMac] CFStringCreateWithCString failed\n";
        return false;
    }

    CFArrayRef pathsToWatch = CFArrayCreate(nullptr, reinterpret_cast<const void**>(&cfPath), 1, nullptr);
    if (!pathsToWatch) {
        CFRelease(cfPath);
        std::cerr << "[FileWatcherMac] CFArrayCreate failed\n";
        return false;
    }

    FSEventStreamContext context{};
    context.version = 0;
    context.info = this; // allow callback to access this instance

    const CFAbsoluteTime latency = 0.5;
    uint32_t flags = kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagUseCFTypes;

    streamRef = FSEventStreamCreate(
        nullptr,
        &FileWatcherMac::s_fsevents_callback,
        &context,
        pathsToWatch,
        kFSEventStreamEventIdSinceNow,
        latency,
        flags
    );

    CFRelease(cfPath);
    CFRelease(pathsToWatch);

    if (!streamRef) {
        std::cerr << "[FileWatcherMac] FSEventStreamCreate returned null\n";
        return false;
    }

    // create dispatch queue for callbacks
    dispatchQueue = dispatch_queue_create("com.filemanager.fsevents", DISPATCH_QUEUE_SERIAL);
    if (!dispatchQueue) {
        FSEventStreamInvalidate(streamRef);
        FSEventStreamRelease(streamRef);
        streamRef = nullptr;
        std::cerr << "[FileWatcherMac] dispatch_queue_create failed\n";
        return false;
    }

    // attach stream to queue and start
    FSEventStreamSetDispatchQueue(streamRef, dispatchQueue);

    if (!FSEventStreamStart(streamRef)) {
        FSEventStreamSetDispatchQueue(streamRef, nullptr);
        FSEventStreamInvalidate(streamRef);
        FSEventStreamRelease(streamRef);
        streamRef = nullptr;
        std::cerr << "[FileWatcherMac] FSEventStreamStart failed\n";
        return false;
    }

    return true;
}

void FileWatcherMac::printFlags(uint32_t flags) {
    if (flags & kFSEventStreamEventFlagItemCreated)     std::cout << "  [Created]";
    if (flags & kFSEventStreamEventFlagItemRemoved)     std::cout << "  [Removed]";
    if (flags & kFSEventStreamEventFlagItemModified)    std::cout << "  [Modified]";
    if (flags & kFSEventStreamEventFlagItemRenamed)     std::cout << "  [Renamed]";
    if (flags & kFSEventStreamEventFlagItemIsFile)      std::cout << "  [File]";
    if (flags & kFSEventStreamEventFlagItemIsDir)       std::cout << "  [Directory]";
    if (flags) std::cout << "\n";
}

// Map fsevent flags -> our EventType mask (simple mapping)
static inline EventType flagsToEventType(uint32_t flags) {
    EventType t = EventType::None;
    if (flags & kFSEventStreamEventFlagItemCreated) t = t | EventType::Created;
    if (flags & kFSEventStreamEventFlagItemRemoved) t = t | EventType::Removed;
    if (flags & kFSEventStreamEventFlagItemModified) t = t | EventType::Modified;
    if (flags & kFSEventStreamEventFlagItemRenamed) t = t | EventType::Renamed;
    return t;
}

bool FileWatcherMac::flagsMatchMask(uint32_t fseFlags) const {
    EventType ev = flagsToEventType(fseFlags);
    return static_cast<uint32_t>(ev & filterMask) != 0;
}

void FileWatcherMac::s_fsevents_callback(
    ConstFSEventStreamRef,
    void* clientCallBackInfo,
    size_t numEvents,
    void* eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId[]
) {
    auto* watcher = static_cast<FileWatcherMac*>(clientCallBackInfo);
    if (!watcher || !watcher->running.load()) return;

    if (eventPaths) {
        CFArrayRef pathsArray = static_cast<CFArrayRef>(eventPaths);
        for (size_t i = 0; i < numEvents; ++i) {
            CFStringRef cfPath = static_cast<CFStringRef>(CFArrayGetValueAtIndex(pathsArray, i));
            if (!cfPath) continue;

            char buffer[PATH_MAX];
            Boolean ok = CFStringGetCString(cfPath, buffer, sizeof(buffer), kCFStringEncodingUTF8);
            std::string pathStr = ok ? std::string(buffer) : std::string();
            uint32_t flags = static_cast<uint32_t>(eventFlags[i]);

            // optional debug print
            std::cout << "[macOS Event] Path: " << pathStr << " Flags: " << flags;
            printFlags(flags);

            // filter by mask
            if (!watcher->flagsMatchMask(flags)) {
                continue; // ignore event
            }

            if (watcher->callback) {
                FileEvent evt;
                evt.path = pathStr;
                evt.flags = flags;
                watcher->callback(evt);
            }
        }
    } else {
        // fallback to C-string array (rare with UseCFTypes)
        char** paths = static_cast<char**>(eventPaths);
        for (size_t i = 0; i < numEvents; ++i) {
            std::string pathStr(paths[i]);
            uint32_t flags = static_cast<uint32_t>(eventFlags[i]);
            std::cout << "[macOS Event] Path: " << pathStr << " Flags: " << flags;
            printFlags(flags);

            if (!watcher->flagsMatchMask(flags)) continue;

            if (watcher->callback) {
                FileEvent evt;
                evt.path = pathStr;
                evt.flags = flags;
                watcher->callback(evt);
            }
        }
    }
}

void FileWatcherMac::setCallback(EventCallback cb) {
    callback = std::move(cb);
}


/*
================= FileWatcherMac - Mini Summary =================

Path → weakly_canonical/absolute → absolutePath
      ↓
FSEventStreamCreate(...) → streamRef
      ↓
FSEventStreamSetDispatchQueue(streamRef, queue)
      ↓
FSEventStreamStart(streamRef)
      ↓
   [macOS filesystem events]
      ↓
s_fsevents_callback(...)
      ↓
Convert CF path → std::string
      ↓
flagsToEventType(flags) → filterMask check
      ↓
If matched → user callback(FileEvent)

stop() cleans:
    - FSEventStreamStop
    - _SetDispatchQueue(nullptr)
    - _Invalidate
    - _Release

=================================================================
*/
