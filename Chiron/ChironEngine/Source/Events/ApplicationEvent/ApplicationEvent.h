#pragma once
#include "Events/Event.h"

namespace Chiron
{
	class ApplicationEvent : Event
	{
	public:
		ApplicationEvent(const std::string& name);
	};
}