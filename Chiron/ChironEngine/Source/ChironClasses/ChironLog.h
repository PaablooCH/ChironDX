#pragma once

#ifndef ENGINE
	#define LOG_TRACE(...)	// Ignore it
	#define LOG_DEBUG(...)// Ignore it
	#define LOG_INFO(...)// Ignore it
	#define LOG_WARNING(...)// Ignore it
	#define LOG_ERROR(...)// Ignore it
	#define LOG_FATAL(...)// Ignore it
#else

#include "spdlog/spdlog.h"

	#define LOG_TRACE(format, ...)		SPDLOG_LOGGER_TRACE(ChironLog::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_DEBUG(format, ...)		SPDLOG_LOGGER_DEBUG(ChironLog::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_INFO(format, ...)		SPDLOG_LOGGER_INFO(ChironLog::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_WARNING(format, ...)	SPDLOG_LOGGER_WARN(ChironLog::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_ERROR(format, ...)		SPDLOG_LOGGER_ERROR(ChironLog::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_FATAL(format, ...)		SPDLOG_LOGGER_FATAL(ChironLog::GetCoreLogger(), format, __VA_ARGS__);

class ChironLog
{
public:
	ChironLog() = default;
	~ChironLog() = default;

	static void Init();

	inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return _coreLogger; }

private:
	static std::shared_ptr<spdlog::logger> _coreLogger;
};

#endif
