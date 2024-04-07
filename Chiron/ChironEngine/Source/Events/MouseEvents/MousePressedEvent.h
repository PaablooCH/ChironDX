#pragma once
#include "MouseEvent.h"

namespace Chiron
{
    class MousePressedEvent : public MouseEvent
    {
    public:
        MousePressedEvent(const std::string& name);
    };
}