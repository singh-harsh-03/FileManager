//entry point for the application

#include <iostream>
#include "platform/PlatformFactory.h"
#include <filesystem>

int main() {
    PlatformFactory pf;

    auto watcher = pf.createFileWatcher();

    watcher->watch(std::filesystem::absolute("test_dir").string());
    std::cout << "Platform factory initialized successfully.\n";
    return 0;
}
