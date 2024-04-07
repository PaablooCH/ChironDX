#include "Pch.h"
#include "ApplicationEvent.h"

namespace Chiron
{
	ApplicationEvent::ApplicationEvent(const std::string& name) : Event(name, EventType::NONE, EventCategory::APPLICATION)
	{
	}
}

