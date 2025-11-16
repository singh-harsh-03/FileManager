#pragma once
#include <string>
class IMemoryMapper{
public:
    virtual ~IMemoryMapper() = default;

    virtual std::pair<char*, size_t> mapFile(const std::string& path) = 0;
    virtual void unmap(char* add,size_t size) = 0;
};