#include "platform/PlatformFactory.h"
#include "platform/interfaces/IFileOps.h"
#include "platform/interfaces/IFileWatcher.h"
#include "platform/interfaces/IMemoryMapper.h"
#include <memory>


// Mac platform headers (stubs, if missing)
#ifdef FM_PLATFORM_MAC
#include "platform/mac/FileOpsMac.h"
#include "platform/mac/FileWatcherMac.h"
#include "platform/mac/MemoryMapperMac.h"
#endif

#ifdef FM_PLATFORM_LINUX
#include "platform/linux/FileOpsLinux.h"
#include "platform/linux/FileWatcherLinux.h"
#include "platform/linux/MemoryMapperLinux.h"
#endif


std::unique_ptr<IFileOps> PlatformFactory::createFileOps() {
#if defined(FM_PLATFORM_LINUX)
    return std::make_unique<FileOpsLinux>();
#elif defined(FM_PLATFORM_MAC)
    return std::make_unique<FileOpsMac>();
#endif
}

std::unique_ptr<IFileWatcher> PlatformFactory::createFileWatcher() {
#if defined(FM_PLATFORM_LINUX)
    return std::make_unique<FileWatcherLinux>();
#elif defined(FM_PLATFORM_MAC)
    return std::make_unique<FileWatcherMac>();
#endif
}

std::unique_ptr<IMemoryMapper> PlatformFactory::createMemoryMapper() {
#if defined(FM_PLATFORM_LINUX)
    return std::make_unique<MemoryMapperLinux>();
#elif defined(FM_PLATFORM_MAC)
    return std::make_unique<MemoryMapperMac>();
#endif
}
