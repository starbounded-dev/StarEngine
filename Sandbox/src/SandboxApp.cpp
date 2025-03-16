#include <StarEngine.h>
#include <StarEngine/Core/EntryPoint.h>

#include "StarEngine/Renderer/OrthographicCameraController.h"
#include "Sandbox2D.h"

#include "ExampleLayer.h"

class Sandbox : public StarEngine::Application
{
public:
	Sandbox(StarEngine::ApplicationCommandLineArgs args)
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}

};

StarEngine::Application* StarEngine::CreateApplication(StarEngine::ApplicationCommandLineArgs args)
{
	return new Sandbox(args);
}
