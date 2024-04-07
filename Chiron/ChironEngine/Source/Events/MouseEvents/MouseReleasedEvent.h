#pragma once
#include "MouseEvent.h"

namespace Chiron
{
    class MouseReleasedEvent : public MouseEvent
    {
    public:
        MouseReleasedEvent(const std::string& name);
    };
}