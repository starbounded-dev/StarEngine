#pragma once

#ifdef SS_PLATFORM_WINDOWS

extern StarStudio::Application* StarStudio::CreateApplication();

	int main(int argc, char** argv) {
		printf("StarStudio Engine\n");
		auto app = StarStudio::CreateApplication();
		app->Run();
		delete app;
	}
#endif