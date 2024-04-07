#include "Pch.h"
#include "WindowEvent.h"

namespace Chiron
{
	WindowEvent::WindowEvent(const std::string& name, EventType type) : Event(name, type, EventCategory::WINDOW)
	{
	}
}