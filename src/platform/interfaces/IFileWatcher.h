#pragma once
#include <string>
#include <functional>

struct FileEvent {
    std::string path;
    uint32_t flags;
};

// Callback type
using EventCallback = std::function<void(const FileEvent&)>;
class IFileWatcher{
public:
    virtual ~IFileWatcher() = default;
    virtual void setCallback(EventCallback cb) = 0;
    virtual void watch(const std::string& path) = 0;
    virtual void stop() = 0;
};