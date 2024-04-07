#include "Pch.h"
#include "KeyReleasedEvent.h"

namespace Chiron
{
	KeyReleasedEvent::KeyReleasedEvent(const std::string& name) : KeyEvent(name, EventType::KEY_RELEASED)
	{
	}
}