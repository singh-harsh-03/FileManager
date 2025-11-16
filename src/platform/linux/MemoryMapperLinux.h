#pragma once
#include "interfaces/IMemoryMapper.h"

class MemoryMapperLinux : public IMemoryMapper {
public:
    ~MemoryMapperLinux() override = default;
    std::pair<char*, size_t> mapFile(const std::string& path) override { return {nullptr, 0}; }
    void unmap(char* add, size_t size) override {}
};
