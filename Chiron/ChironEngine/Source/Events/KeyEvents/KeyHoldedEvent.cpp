#include "Pch.h"
#include "KeyHoldedEvent.h"

namespace Chiron
{
	KeyHoldedEvent::KeyHoldedEvent(const std::string& name) : KeyEvent(name, EventType::KEY_HOLDED)
	{
	}
}