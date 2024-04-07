#pragma once
#include "WindowEvent.h"

namespace Chiron
{
    class WindowBorderlessEvent : public WindowEvent
    {
    public:
        WindowBorderlessEvent(const std::string& name);
    };
}