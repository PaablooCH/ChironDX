#pragma once
#include "Events/Event.h"

namespace Chiron
{
    class WindowEvent : public Event
    {
    public:
        WindowEvent(const std::string& name, EventType type);
    };
}

