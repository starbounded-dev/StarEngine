#pragma once
#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Application.h"

#ifdef SE_PLATFORM_WINDOWS

extern StarEngine::Application* StarEngine::CreateApplication(ApplicationCommandLineArgs args);

	int main(int argc, char** argv) {
		StarEngine::Log::Init();

		SE_PROFILE_BEGIN_SESSION("Startup", "StarEngineProfile-Startup.json");
		auto app = StarEngine::CreateApplication({ argc, argv });
		SE_PROFILE_END_SESSION();

		SE_PROFILE_BEGIN_SESSION("Runtime", "StarEngineProfile-Runtime.json");
		app->Run();
		SE_PROFILE_END_SESSION();

		SE_PROFILE_BEGIN_SESSION("Shutdown", "StarEngineProfile-Shutdown.json");
		delete app;
		SE_PROFILE_END_SESSION();
	}
#endif
