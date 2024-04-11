#include "Pch.h"
#include "ChironLog.h"

#ifdef ENGINE

#include "spdlog/sinks/stdout_color_sinks.h"

std::shared_ptr<spdlog::logger> ChironLog::_coreLogger;

void ChironLog::Init()
{
	spdlog::set_pattern("%^[%T][%n][%l]: %@ %v%$");
	spdlog::set_level(spdlog::level::trace);

	_coreLogger = spdlog::stdout_color_mt("CHIRON");
	_coreLogger->set_level(spdlog::level::trace);
}
#endif