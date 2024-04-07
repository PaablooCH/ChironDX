#include "Pch.h"
#include "MouseEvent.h"

namespace Chiron
{
	MouseEvent::MouseEvent(const std::string& name, EventType type, EventCategory extraCategory) : Event(name, type, EventCategory::INPUT, EventCategory::MOUSE, extraCategory)
	{
	}
}