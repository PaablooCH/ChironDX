#include "Pch.h"
#include "WindowCloseEvent.h"

namespace Chiron
{
	WindowCloseEvent::WindowCloseEvent(const std::string name) : WindowEvent(name, EventType::WINDOW_CLOSED)
	{
	}
}