#include "Pch.h"
#include "WindowResizedEvent.h"

namespace Chiron
{
	WindowResizedEvent::WindowResizedEvent(const std::string& name, unsigned int width, unsigned int height) :
		WindowEvent(name, EventType::WINDOW_RESIZED), _width(width), _height(height)
	{
	}
}