
#pragma once
#include "platform/interfaces/IFileOps.h"

class FileOpsMac : public IFileOps {
public:
	~FileOpsMac() override = default;
	bool copyFile(const std::string& srcPath, const std::string& destPath) override { return false; }
	bool moveFile(const std::string& srcPath, const std::string& destPath) override { return false; }
	bool deleteFile(const std::string& path) override { return false; }
	bool createDir(const std::string& path) override { return false; }
	bool removeDir(const std::string& path) override { return false; }
};
