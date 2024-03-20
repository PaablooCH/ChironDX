#pragma once

#include "Core.h"

namespace Chiron
{
	class CHIRON_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}


