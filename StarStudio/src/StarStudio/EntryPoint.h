#pragma once

#ifdef SS_PLATFORM_WINDOWS

extern StarStudio::Application* StarStudio::CreateApplication();

	int main(int argc, char** argv) {
		StarStudio::Log::Init();
		SS_CORE_INFO("Engine working!");
		SS_CLIENT_INFO("Hello Client!");

		auto app = StarStudio::CreateApplication();
		app->Run();
		delete app;
	}
#endif