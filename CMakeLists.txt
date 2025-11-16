cmake_minimum_required(VERSION 3.22)
project(FileManager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect OS
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DFM_PLATFORM_LINUX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_definitions(-DFM_PLATFORM_MAC)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

add_subdirectory(src)
