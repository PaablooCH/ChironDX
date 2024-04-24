#pragma once

// extracted from AxolotlEngine

#include "Formatter.h"

enum class LogSeverity
{
	TRACE_LOG,
	DEBUG_LOG,
	INFO_LOG,
	WARNING_LOG,
	ERROR_LOG,
	FATAL_LOG
};

#define LOG_TRACE(format, ...)		CHIRON_LOG(format, LogSeverity::TRACE_LOG, __VA_ARGS__)
#define LOG_DEBUG(format, ...)		CHIRON_LOG(format, LogSeverity::DEBUG_LOG, __VA_ARGS__)
#define LOG_INFO(format, ...)		CHIRON_LOG(format, LogSeverity::INFO_LOG, __VA_ARGS__)
#define LOG_WARNING(format, ...)	CHIRON_LOG(format, LogSeverity::WARNING_LOG, __VA_ARGS__)
#define LOG_ERROR(format, ...)		CHIRON_LOG(format, LogSeverity::ERROR_LOG, __VA_ARGS__)
#define LOG_FATAL(format, ...)		CHIRON_LOG(format, LogSeverity::FATAL_LOG, __VA_ARGS__)
#define CHIRON_LOG(format, severity, ...) logContext->LogMessage(__FILE__, __LINE__, severity, format, __VA_ARGS__)

namespace Chiron
{
	class Log
	{
	public:
		Log() = default;
		~Log();

		template<typename... Args>
		void LogMessage(const char file[], int line, LogSeverity severity, const std::string& format, Args&&... args);

		void Write(const char file[], int line, LogSeverity severity, std::string&& formattedLine);

	private:
		struct LogLine
		{
		public:
			std::string ToString(bool addBreak) const;

		public:
			// Info of each line
			LogSeverity severity;
			std::string file;
			uint16_t line;
			std::string message;
		};

		std::vector<LogLine> _logLines;
	};

	template<typename ...Args>
	inline void Log::LogMessage(const char file[], int line, LogSeverity severity, const std::string& format, Args && ...args)
	{
		Write(file, line, severity, Chiron::Format(format, std::forward<Args>(args)...));
	}
}

extern std::unique_ptr<Chiron::Log> logContext;