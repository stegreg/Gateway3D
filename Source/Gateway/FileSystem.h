#pragma once

#include <string>

namespace Gateway
{

class FileSystem
{
public:
	static void				  SetRootPath(const std::string& rootPath);

	static const std::string& GetRootPath();
	
	static bool				  IsFullPath(const std::string& path);

	static const std::string  GetFullPath(const std::string& path);

	static void				  MakeDirectories(const std::string& path);

private:
	static std::string s_rootPath;
};

}