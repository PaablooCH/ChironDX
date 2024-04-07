#include "Pch.h"
#include "MouseReleasedEvent.h"

namespace Chiron
{
	MouseReleasedEvent::MouseReleasedEvent(const std::string& name) : MouseEvent(name, EventType::MOUSE_RELEASED, EventCategory::MOUSE_BUTTON)
	{
	}
}