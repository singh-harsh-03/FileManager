#pragma once
#include <memory>
class FileManager {
public:
    FileManager(std::unique_ptr<IPlatformFactory> factory);

    bool copy(const std::string& src, const std::string& dst);
    bool move(const std::string& src, const std::string& dst);
    bool remove(const std::string& path);

    std::vector<std::string> listDir(const std::string& path);
private:
    std::unique_ptr<IFileOps> fileOps;
    std::unique_ptr<IFileWatcher> watcher;
    std::unique_ptr<IMemoryMapper> mapper;
};
