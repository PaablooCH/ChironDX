#pragma once

#ifdef CRN_PLATFORM_WINDOW

#include <memory>
#include "ChironClasses/Log.h"
#include "Application.h"

extern Chiron::Application* Chiron::CreateApplication();

std::unique_ptr<Chiron::Application> app(Chiron::CreateApplication());

#ifdef ENGINE
std::unique_ptr<Chiron::Log> logContext = std::make_unique<Chiron::Log>();
#endif // ENGINE

int main(int argc, char** argv)
{
	Chiron::Log::Init();
	LOG_TRACE("aaaaa");
	LOG_WARNING("aaaaa");

	app->Run();
	return 1;
}

#else
#error Chiron only suppports Windows right now!
#endif // CRN_PLATFORM_WINDOW