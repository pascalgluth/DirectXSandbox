#pragma once
#include <string>

#define LOG_INFO(str) FLogger::LogInfo(std::string(str) + "\n")
#define LOG_WARN(str) FLogger::LogWarn(std::string(str) + "\n")
#define LOG_ERROR(str) FLogger::LogError(std::string(str) + "\n")
#define LOG_ERROR_HR(str,hr) FLogger::LogError(std::string(str) + " (" + std::to_string(hr) + ")\n")

namespace FLogger
{
	void LogInfo(const std::string& str);
	void LogWarn(const std::string& str);
	void LogError(const std::string& str);
}
