#pragma once

#include "interfaces/IPlatformFactory.h"

class PlatformFactory : public IPlatformFactory {
public:
    std::unique_ptr<IFileOps> createFileOps() override;
    std::unique_ptr<IFileWatcher> createFileWatcher() override;
    std::unique_ptr<IMemoryMapper> createMemoryMapper() override;
};
