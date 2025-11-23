//entry point for the application

#include <iostream>
#include "platform/PlatformFactory.h"
#include <filesystem>
#include <dispatch/dispatch.h>

int main() {
    PlatformFactory pf;
    auto watcher = pf.createFileWatcher();

    watcher->setCallback([](const FileEvent& evt){
        std::cout << "EVENT: " << evt.path << " flags=" << evt.flags << "\n";
    });
    watcher->watch(std::filesystem::absolute("test_dir").string());
    std::cout << "Platform factory initialized successfully.\n";
    dispatch_main(); // <---- keeps process alive forever
}
