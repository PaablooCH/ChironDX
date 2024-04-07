#pragma once
#include "WindowEvent.h"

namespace Chiron
{
    class WindowFullScreenEvent : public WindowEvent
    {
    public:
        WindowFullScreenEvent(const std::string& name);
    };
}