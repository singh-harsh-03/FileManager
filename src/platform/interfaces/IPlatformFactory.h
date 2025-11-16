#pragma once
#include <memory>
#include "IFileOps.h"
#include "IFileWatcher.h"
#include "IMemoryMapper.h"
class IPlatformFactory {
public:
    virtual ~IPlatformFactory() = default;

    virtual std::unique_ptr<IFileOps> createFileOps() = 0;
    virtual std::unique_ptr<IFileWatcher> createFileWatcher() = 0;
    virtual std::unique_ptr<IMemoryMapper> createMemoryMapper() = 0;
};
