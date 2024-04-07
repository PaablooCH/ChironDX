#pragma once

#include "KeyEvent.h"

namespace Chiron
{
    class KeyHoldedEvent : public KeyEvent
    {
    public:
        KeyHoldedEvent(const std::string& name);
    };
}

