#include "Pch.h"
#include "MouseMoveEvent.h"

namespace Chiron
{
	MouseMoveEvent::MouseMoveEvent(const std::string& name) : MouseEvent(name, EventType::MOUSE_MOVE)
	{
	}
}