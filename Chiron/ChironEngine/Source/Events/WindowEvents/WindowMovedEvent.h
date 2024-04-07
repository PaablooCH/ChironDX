#pragma once
#include "WindowEvent.h"

namespace Chiron
{
    class WindowMovedEvent : public WindowEvent
    {
    public:
        WindowMovedEvent(const std::string& name);
    };
}