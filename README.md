# FileManager

A cross-platform file management library for Linux and macOS. Built with C++17 using the Abstract Factory pattern to handle platform-specific implementations.

## What It Does

FileManager provides a unified interface for:
- File operations (copy, move, delete)
- File system watching (monitoring changes in real-time)
- Memory-mapped file I/O

Write your code once, and it works on both Linux and macOS without platform-specific conditionals.


## Architecture

### High-Level Architecture

```
┌─────────────────────────────────────────────────┐
│           Application Layer                      │
│         (Platform-Independent Code)              │
└───────────────────┬─────────────────────────────┘
                    │
                    ↓ Uses Interfaces
┌─────────────────────────────────────────────────┐
│          Interface Layer (Abstract)              │
├─────────────────────────────────────────────────┤
│  • IFileOps      - File operations              │
│  • IFileWatcher  - File system monitoring       │
│  • IMemoryMapper - Memory-mapped I/O            │
│  • IPlatformFactory - Object creation           │
└───────────────────┬─────────────────────────────┘
                    │
                    ↓ Implemented by
┌─────────────────────────────────────────────────┐
│         Platform Factory (Selector)              │
│  Compile-time selection of implementations      │
└────────────┬────────────────────┬────────────────┘
             │                    │
             ↓                    ↓
┌──────────────────────┐  ┌──────────────────────┐
│   Linux Platform     │  │    Mac Platform      │
├──────────────────────┤  ├──────────────────────┤
│ • FileOpsLinux       │  │ • FileOpsMac         │
│ • FileWatcherLinux   │  │ • FileWatcherMac     │
│ • MemoryMapperLinux  │  │ • MemoryMapperMac    │
└──────────────────────┘  └──────────────────────┘
```


The factory pattern selects the right implementation at compile time based on your platform. No runtime overhead, just clean abstraction.

## Project Structure

```
FileManager/
├── src/
│   ├── main.cpp
│   ├── platform/
│   │   ├── interfaces/          # Abstract interfaces
│   │   ├── linux/               # Linux implementations
│   │   ├── mac/                 # macOS implementations
│   │   ├── PlatformFactory.h
│   │   └── PlatformFactory.cpp
│   ├── core/                    # Business logic
│   └── cli/                     # CLI tool
└── CMakeLists.txt
```

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
./FileManager
```

CMake automatically detects your platform and sets the right flags:
- Linux: `-DFM_PLATFORM_LINUX`
- macOS: `-DFM_PLATFORM_MAC`

On macOS, CoreServices and CoreFoundation frameworks are linked automatically.

## Current Status

**Working:**
- macOS file watcher using FSEvents API
- Platform factory and interface structure
- Basic event monitoring

**Not Implemented:**
- Linux file watcher (needs inotify)
- File operations (copy, move, delete, etc.)
- Memory mappers
- Threading for file watchers
- Custom callbacks
- Tests

## macOS File Watcher

Uses Apple's FSEvents API. Key concepts:

**FSEventStream**: A continuous subscription to file system events. You create it, schedule it with a run loop, and it calls your callback when files change.

**CFRunLoop**: An event processing loop. It sits and waits for events (file changes, timers, network activity) and dispatches them efficiently instead of busy-waiting.

**CFString**: Core Foundation's string type. It's C-based and requires manual memory management with `CFRelease()`.

Current implementation blocks the calling thread. Next step is running it in a separate thread.

## Todo

**Phase 1: Platform Implementation**
- [ ] Implement Linux file watcher with inotify
- [ ] Implement file operations (copy, move, delete, mkdir, rmdir)
- [ ] Implement memory mappers using mmap/munmap
- [ ] Add threading to file watchers
- [ ] Add callback registration system

**Phase 2: Core File Manager Features**
- [ ] File search (by name, extension, size, date)
- [ ] Directory traversal and listing
- [ ] File metadata operations (permissions, timestamps, ownership)
- [ ] Symbolic link handling
- [ ] File size and disk usage calculations
- [ ] File comparison (content, checksums)
- [ ] Compression/decompression (zip, tar, gz)
- [ ] File encryption/decryption
- [ ] Duplicate file detection
- [ ] Trash/recycle bin functionality

**Phase 3: Advanced Features**
- [ ] Recursive directory watching
- [ ] Event filtering and pattern matching
- [ ] File synchronization between directories
- [ ] Batch operations with progress tracking
- [ ] File versioning/backup system
- [ ] Network file operations (FTP, SFTP, cloud storage)
- [ ] File preview generation (thumbnails, text preview)
- [ ] Smart file organization (auto-categorization)
- [ ] File indexing for fast search
- [ ] Plugin system for custom operations

**Phase 4: Developer Experience**
- [ ] Error handling framework
- [ ] Logging system
- [ ] Unit tests
- [ ] Integration tests
- [ ] Documentation and examples
- [ ] Performance benchmarks
- [ ] CLI tool
- [ ] Configuration system

**Phase 5: Platform Expansion**
- [ ] Windows support
- [ ] Android support
- [ ] iOS support

## macOS File Watcher


## How It Works

The PlatformFactory uses preprocessor directives to compile different code per platform:

```cpp
std::unique_ptr PlatformFactory::createFileWatcher() {
#if defined(FM_PLATFORM_LINUX)
    return std::make_unique();
#elif defined(FM_PLATFORM_MAC)
    return std::make_unique();
#endif
}
```

application code just does:
```cpp
PlatformFactory factory;
auto watcher = factory.createFileWatcher();
watcher->watch("./path");
```

The compiler generates the right code for your platform. Zero runtime overhead.



### CMake Configuration

The build system automatically detects your platform and defines appropriate macros:

- **Linux**: `-DFM_PLATFORM_LINUX`
- **macOS**: `-DFM_PLATFORM_MAC`

On macOS, the following frameworks are automatically linked:
- `CoreServices` (for FSEvents)
- `CoreFoundation` (for CF types)
