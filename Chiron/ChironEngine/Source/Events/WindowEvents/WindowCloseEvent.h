#pragma once
#include "WindowEvent.h"

namespace Chiron
{
    class WindowCloseEvent : public WindowEvent
    {
    public:
        WindowCloseEvent(const std::string name);
    };
}