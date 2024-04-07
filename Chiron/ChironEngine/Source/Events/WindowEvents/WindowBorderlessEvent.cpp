#include "Pch.h"
#include "WindowBorderlessEvent.h"

namespace Chiron
{
	WindowBorderlessEvent::WindowBorderlessEvent(const std::string& name) : WindowEvent(name, EventType::WINDOW_BORDERLESS)
	{
	}
}