#include "Pch.h"
#include "Log.h"

#include <mutex>

namespace
{
	std::recursive_mutex writeLock; // block write in multithread
}

namespace Chiron
{
	Log::~Log()
	{
		_logLines.clear();
	}

	void Log::Write(const char file[], int line, LogSeverity severity, std::string&& formattedLine)
	{
		std::scoped_lock lock(writeLock);

		LogLine logLine{ severity, file, static_cast<uint16_t>(line), std::move(formattedLine) };
		_logLines.push_back(logLine);

		std::string toString = logLine.ToString(true);
		OutputDebugStringA(toString.c_str());
	}

	std::string Log::LogLine::ToString(bool addBreak) const
	{
		std::string result;
		switch (severity)
		{
		case LogSeverity::TRACE_LOG:
			result = "[TRACE]\t";
			break;
		case LogSeverity::DEBUG_LOG:
			result = "[DEBUG]\t";
			break;
		case LogSeverity::INFO_LOG:
			result = "[INFO]\t";
			break;
		case LogSeverity::WARNING_LOG:
			result = "[WARNING]\t";
			break;
		case LogSeverity::ERROR_LOG:
			result = "[ERROR]\t";
			break;
		case LogSeverity::FATAL_LOG:
			result = "[FATAL]\t";
			break;
		}

		result += file + "(" + std::to_string(line) + ") : ";

		result += message;
		if (addBreak)
		{
			result += '\n';
		}
		return result;
	}
}