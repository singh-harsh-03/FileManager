#pragma once
#include <string>
class IFileOps {
public:
    virtual ~IFileOps() = default;

    virtual bool copyFile(const std::string& srcPath, const std::string& destPath) = 0;
    virtual bool moveFile(const std::string& srcPath, const std::string& destPath) = 0;
    virtual bool deleteFile(const std::string& path) = 0;

    virtual bool createDir(const std::string& path) = 0;
    virtual bool removeDir(const std::string& path) = 0;
    
};