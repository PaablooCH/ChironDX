#pragma once

#include "KeyEvent.h"

namespace Chiron
{
    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(const std::string& name);
    };
}