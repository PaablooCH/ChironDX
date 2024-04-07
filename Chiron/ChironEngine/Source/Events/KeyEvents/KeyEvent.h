#pragma once

#include "Events/Event.h"

namespace Chiron
{
    class KeyEvent : public Event
    {
    public:
        KeyEvent(const std::string& name, EventType type);
    };
}