#pragma once

#ifndef ENGINE
	#define LOG_TRACE(...)	// Ignore it
	#define LOG_DEBUG(...)// Ignore it
	#define LOG_INFO(...)// Ignore it
	#define LOG_WARNING(...)// Ignore it
	#define LOG_ERROR(...)// Ignore it
	#define LOG_FATAL(...)// Ignore it
#else

#include "Core.h"
#include "spdlog/spdlog.h"

#ifdef CORE
	#define LOG_TRACE(format, ...)		SPDLOG_LOGGER_TRACE(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_DEBUG(format, ...)		SPDLOG_LOGGER_DEBUG(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_INFO(format, ...)		SPDLOG_LOGGER_INFO(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_WARNING(format, ...)	SPDLOG_LOGGER_WARN(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_ERROR(format, ...)		SPDLOG_LOGGER_ERROR(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
	#define LOG_FATAL(format, ...)		SPDLOG_LOGGER_FATAL(Chiron::Log::GetCoreLogger(), format, __VA_ARGS__);
#else
	#define LOG_TRACE(format, ...)		SPDLOG_LOGGER_TRACE(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
	#define LOG_DEBUG(format, ...)		SPDLOG_LOGGER_DEBUG(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
	#define LOG_INFO(format, ...)		SPDLOG_LOGGER_INFO(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
	#define LOG_WARNING(format, ...)	SPDLOG_LOGGER_WARN(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
	#define LOG_ERROR(format, ...)		SPDLOG_LOGGER_ERROR(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
	#define LOG_FATAL(format, ...)		SPDLOG_LOGGER_FATAL(Chiron::Log::GetClientLogger(), format, __VA_ARGS__);
#endif // CORE

namespace Chiron {
	class CHIRON_API Log
	{
	public:
		Log() = default;
		~Log() = default;

		static void Init();

		inline static std::shared_ptr<spdlog::logger> GetCoreLogger() { return _coreLogger; }
		inline static std::shared_ptr<spdlog::logger> GetClientLogger() { return _clientLogger; }

	private:
		static std::shared_ptr<spdlog::logger> _coreLogger;
		static std::shared_ptr<spdlog::logger> _clientLogger;
	};
}
#endif
