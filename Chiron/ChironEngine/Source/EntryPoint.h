#pragma once

#ifdef CRN_PLATFORM_WINDOW

#include <memory>
#include "ChironClasses/ChironLog.h"
#include "Application.h"

extern Chiron::Application* Chiron::CreateApplication();

std::unique_ptr<Chiron::Application> app (Chiron::CreateApplication());

#ifdef ENGINE
std::unique_ptr<Chiron::ChironLog> logContext = std::make_unique<Chiron::ChironLog>();
#endif // ENGINE

int main(int argc, char** argv)
{
    app->Run();
    return 1;
}

#else
#error Chiron only suppports Windows right now!
#endif // CRN_PLATFORM_WINDOW