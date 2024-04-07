#include "Pch.h"
#include "WindowMovedEvent.h"

namespace Chiron
{
	WindowMovedEvent::WindowMovedEvent(const std::string& name) : WindowEvent(name, EventType::WINDOW_MOVED)
	{
	}
}

