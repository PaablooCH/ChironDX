#pragma once

#ifndef ENGINE
	#define LOG_INFO(...)	 // ignore it
	#define LOG_VERBOSE(...) // ignore it
	#define LOG_DEBUG(...)	 // ignore it
	#define LOG_WARNING(...) // ignore it
	#define LOG_ERROR(...)	 // ignore it
	#define CHIRON_LOG(...)	 // ignore it
#else
#include "Auxiliar/Formatter.h"

	#define LOG_TRACE(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::TRACE_LOG, __VA_ARGS__)
	#define LOG_DEBUG(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::DEBUG_LOG, __VA_ARGS__)
	#define LOG_INFO(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::INFO_LOG, __VA_ARGS__)
	#define LOG_WARNING(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::WARNING_LOG, __VA_ARGS__)
	#define LOG_ERROR(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::ERROR_LOG, __VA_ARGS__)
	#define LOG_FATAL(format, ...) CHIRON_LOG(format, Chiron::LogSeverity::FATAL_LOG, __VA_ARGS__)
	#define CHIRON_LOG(format, severity, ...) logContext->Log(__FILE__, __LINE__, severity, format, __VA_ARGS__)

namespace Chiron {

	enum class LogSeverity
	{
		TRACE_LOG,
		DEBUG_LOG,
		INFO_LOG,
		WARNING_LOG,
		ERROR_LOG,
		FATAL_LOG
	};	

	class ChironLog
	{
	public:
		ChironLog() = default;
		~ChironLog();

		template<typename... Args>
		void Log(const char file[], int line, LogSeverity severity, const std::string& format, Args&&... args);

	private:
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
	inline void ChironLog::Log(const char file[], int line, LogSeverity severity, const std::string& format, Args && ...args)
	{
		Write(file, line, severity, chiron::Format(format, std::forward<Args>(args)...));
	}
}

#endif
