#pragma once
#include <string>

#define LOG_INFO(str) Logger::LogInfo(std::string(str) + "\n")
#define LOG_WARN(str) Logger::LogWarn(std::string(str) + "\n")
#define LOG_ERROR(str) Logger::LogError(std::string(str) + "\n")
#define LOG_ERROR_HR(str,hr) Logger::LogError(std::string(str) + " (" + std::to_string(hr) + ")\n")

namespace Logger
{
	void LogInfo(const std::string& str);
	void LogWarn(const std::string& str);
	void LogError(const std::string& str);
}
