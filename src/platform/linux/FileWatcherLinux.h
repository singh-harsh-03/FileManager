#pragma once
#include "../interfaces/IFileWatcher.h"
#include <atomic>

class FileWatcherLinux : public IFileWatcher {
public:
    FileWatcherLinux();
    ~FileWatcherLinux() override;

    void watch(const std::string& path) override;
    void stop() override;

private:
    int inotifyFd;
    int watchFd;
    std::atomic<bool> running;
};
