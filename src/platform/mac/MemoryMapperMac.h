#pragma once
#include "platform/interfaces/IMemoryMapper.h"

class MemoryMapperMac : public IMemoryMapper {
public:
    ~MemoryMapperMac() override = default;
    std::pair<char*, size_t> mapFile(const std::string& path) override { return {nullptr, 0}; }
    void unmap(char* add, size_t size) override {}
};
