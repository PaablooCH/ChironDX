#include "Pch.h"
#include "MousePressedEvent.h"

namespace Chiron
{
	MousePressedEvent::MousePressedEvent(const std::string& name) : MouseEvent(name, EventType::MOUSE_PRESSED, EventCategory::MOUSE_BUTTON)
	{
	}
}