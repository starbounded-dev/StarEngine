#pragma once

#include "Core.h"

namespace StarStudio
{
	class STARSTUDIO_API Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}


