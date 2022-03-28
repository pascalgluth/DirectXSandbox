#include "Logger.h"

#include <Windows.h>

#include "Gui.h"

namespace FLogger
{
	void LogInfo(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
		Gui::AddLogLine(str, Gui::MSG_INFO);
	}

	void LogWarn(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
		Gui::AddLogLine(str, Gui::MSG_WARNING);
	}

	void LogError(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
		Gui::AddLogLine(str, Gui::MSG_ERROR);
	}
}
