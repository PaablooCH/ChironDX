#include "Pch.h"
#include "MouseScrollEvent.h"

namespace Chiron
{
	MouseScrollEvent::MouseScrollEvent(const std::string& name) : MouseEvent(name, EventType::MOUSE_SCROLL)
	{
	}
}