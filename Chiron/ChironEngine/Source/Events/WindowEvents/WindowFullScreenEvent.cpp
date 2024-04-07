#include "Pch.h"
#include "WindowFullScreenEvent.h"

namespace Chiron
{
	WindowFullScreenEvent::WindowFullScreenEvent(const std::string& name) : WindowEvent(name, EventType::WINDOW_FULLSCREEN)
	{
	}
}