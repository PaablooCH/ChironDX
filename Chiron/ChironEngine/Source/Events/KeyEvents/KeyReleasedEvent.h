#pragma once

#include "KeyEvent.h"

namespace Chiron
{
    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(const std::string& name);
    };
}

