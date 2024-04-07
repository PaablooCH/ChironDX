#pragma once
#include "MouseEvent.h"

namespace Chiron
{
    class MouseMoveEvent : public MouseEvent
    {
    public:
        MouseMoveEvent(const std::string& name);
    };
}

