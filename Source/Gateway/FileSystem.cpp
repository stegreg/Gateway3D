#include "../Gateway/FileSystem.h"

#include <filesystem>

namespace Gateway
{

std::string FileSystem::s_rootPath = "GameData";

void FileSystem::SetRootPath(const std::string& rootPath)
{
	s_rootPath = rootPath;
}

const std::string& FileSystem::GetRootPath()
{
	return s_rootPath;
}

bool FileSystem::IsFullPath(const std::string& pathStr)
{
	std::filesystem::path path = pathStr;

	std::filesystem::path startPath = *path.begin();

	return startPath.compare(s_rootPath) == 0;
}

const std::string FileSystem::GetFullPath(const std::string& path)
{
	if ( IsFullPath(path) )
		return path;

	return (std::filesystem::path(s_rootPath)/std::filesystem::path(path)).string();
}

void FileSystem::MakeDirectories(const std::string& pathStr)
{
	std::string fullPathStr = pathStr;

	if ( !IsFullPath(fullPathStr) )
	{
		fullPathStr = GetFullPath(fullPathStr);
	}

	std::filesystem::path path = fullPathStr;

	path.remove_filename();

	std::filesystem::create_directories(path);
}

}