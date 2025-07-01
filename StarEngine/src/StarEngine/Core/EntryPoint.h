#pragma once

#include "StarEngine/Core/Application.h"
#include "StarEngine/Core/Assert.h"

extern StarEngine::Application* StarEngine::CreateApplication(int argc, char** argv);
extern bool g_ApplicationRunning;

namespace StarEngine {

	int Main(int argc, char** argv)
	{
		while (g_ApplicationRunning)
		{
			InitializeCore();

			Application* app = CreateApplication(argc, argv);
			SE_CORE_ASSERT(app, "Client Application is null!");

			app->Run();

			delete app;
			ShutdownCore();
		}
		return 0;
	}

}

#if defined(SE_DIST) && defined(SE_PLATFORM_WINDOWS)
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	return StarEngine::Main(__argc, __argv);
}
#else
int main(int argc, char** argv)
{
	return StarEngine::Main(argc, argv);
}
#endif
