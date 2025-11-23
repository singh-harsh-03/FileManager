#pragma once
#include <string>
#include <functional>

struct FileEvent {
    std::string path;
    uint32_t flags;
};

// Callback type
using EventCallback = std::function<void(const FileEvent&)>;

enum class EventType : uint32_t {
    None    = 0,
    Created = 1 << 0,
    Modified= 1 << 1,
    Removed = 1 << 2,
    Renamed = 1 << 3,
    All     = 0xFFFFFFFF
};

inline EventType operator|(EventType a, EventType b) {
    return static_cast<EventType>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}
inline EventType operator&(EventType a, EventType b) {
    return static_cast<EventType>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

class IFileWatcher{
public:
    virtual ~IFileWatcher() = default;
    virtual void setCallback(EventCallback cb) = 0;
    virtual void watch(const std::string& path) = 0;
    virtual void stop() = 0;
    virtual void setFilter(EventType mask) = 0;     // Set filter mask (Created/Modified/Removed/Renamed). Default is EventType::All.

};