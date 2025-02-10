#pragma once
#include "StarStudio/Core/Base.h"

#ifdef SS_PLATFORM_WINDOWS

extern StarStudio::Application* StarStudio::CreateApplication();

	int main(int argc, char** argv) {
		StarStudio::Log::Init();

		SS_PROFILE_BEGIN_SESSION("Startup", "StarStudioProfile-Startup.json");
		auto app = StarStudio::CreateApplication();
		SS_PROFILE_END_SESSION();

		SS_PROFILE_BEGIN_SESSION("Runtime", "StarStudioProfile-Runtime.json");
		app->Run();
		SS_PROFILE_END_SESSION();

		SS_PROFILE_BEGIN_SESSION("Shutdown", "StarStudioProfile-Shutdown.json");
		delete app;
		SS_PROFILE_END_SESSION();
	}
#endif