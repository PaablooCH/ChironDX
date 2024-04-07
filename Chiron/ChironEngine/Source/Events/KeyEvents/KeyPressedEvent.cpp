#include "Pch.h"
#include "KeyPressedEvent.h"

namespace Chiron
{
	KeyPressedEvent::KeyPressedEvent(const std::string& name) : KeyEvent(name, EventType::KEY_PRESSED)
	{
	}
}