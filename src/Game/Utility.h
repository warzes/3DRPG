#pragma once

inline std::string GetFileExtension(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	return path.extension().string();
}

inline std::string GetFileDirectory(const std::string& filePath)
{
	std::filesystem::path path(filePath);
	return path.parent_path().string() + "/";
}