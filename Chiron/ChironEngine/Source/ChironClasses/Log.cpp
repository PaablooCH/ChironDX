#include "Pch.h"

#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Chiron
{
	std::shared_ptr<spdlog::logger> Log::_coreLogger;
	std::shared_ptr<spdlog::logger> Log::_clientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T][%n][%l]: %@ %v%$");
		spdlog::set_level(spdlog::level::trace);

		_coreLogger = spdlog::stdout_color_mt("CHIRON");
		_coreLogger->set_level(spdlog::level::trace);

		_clientLogger = spdlog::stdout_color_mt("APP");
		_clientLogger->set_level(spdlog::level::trace);
	}
}