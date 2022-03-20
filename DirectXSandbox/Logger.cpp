#include "Logger.h"
#include <Windows.h>

namespace Logger
{
	void LogInfo(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
	}

	void LogWarn(const std::string& str)
	{
		// TODO: Yellow color
		LogInfo(str);
	}

	void LogError(const std::string& str)
	{
		// TODO: Red color
		LogInfo(str);
	}
}
