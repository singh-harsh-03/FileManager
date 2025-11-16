#pragma once
#include <string>

class IFileWatcher{
public:
    virtual ~IFileWatcher() = default;
    
    virtual void watch(const std::string& path) = 0;
    virtual void stop() = 0;
};