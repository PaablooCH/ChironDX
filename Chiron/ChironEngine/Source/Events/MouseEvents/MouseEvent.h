#pragma once
#include "Events/Event.h"

namespace Chiron 
{
    class MouseEvent : public Event
    {
    public:
        MouseEvent(const std::string& name, EventType type, EventCategory extraCategory = EventCategory::NONE);
    };
}



