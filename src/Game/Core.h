#pragma once

void Print(const std::string& msg);
void Warning(const std::string& msg);
void Error(const std::string& msg);
void Fatal(const std::string& msg);

void ExitApp();

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