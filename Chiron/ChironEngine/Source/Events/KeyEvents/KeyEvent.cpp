#include "Pch.h"
#include "KeyEvent.h"

namespace Chiron
{
	KeyEvent::KeyEvent(const std::string& name, EventType type) : Event(name, type, EventCategory::INPUT, EventCategory::KEYBOARD)
	{
	}
}
