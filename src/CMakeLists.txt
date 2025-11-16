file(GLOB_RECURSE CORE_SRC core/*.cpp)
file(GLOB_RECURSE PLATFORM_SRC platform/*.cpp)
file(GLOB_RECURSE CLI_SRC cli/*.cpp)

add_library(FileManager_lib
    ${CORE_SRC}
    ${PLATFORM_SRC}
    ${CLI_SRC}
)

target_include_directories(FileManager_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE)
    target_link_libraries(FileManager_lib
        "-framework CoreServices"
        "-framework CoreFoundation"
    )
endif()

add_executable(FileManager main.cpp)
target_link_libraries(FileManager PRIVATE FileManager_lib)

