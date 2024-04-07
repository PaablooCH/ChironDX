#pragma once
#include "MouseEvent.h"

namespace Chiron
{
    class MouseScrollEvent : public MouseEvent
    {
    public:
        MouseScrollEvent(const std::string& name);
    };
}


