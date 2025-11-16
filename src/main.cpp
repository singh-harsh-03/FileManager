//entry point for the application

#include <iostream>
#include "platform/PlatformFactory.h"

int main() {
    PlatformFactory pf;

    auto watcher = pf.createFileWatcher();

    watcher->watch("./test_dir");
    std::cout << "Platform factory initialized successfully.\n";
    return 0;
}
