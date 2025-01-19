#include "sspch.h"

#include "Application.h"

#include "StarStudio/Events/ApplicationEvent.h"
#include "StarStudio/Log.h"

namespace StarStudio
{
	Application::Application()
	{

	}
	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			SS_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			SS_TRACE(e);
		}

		while (true);
		{
			// Do something
		}
	}
}


