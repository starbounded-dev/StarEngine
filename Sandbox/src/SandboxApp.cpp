#include <StarEngine.h>
#include <StarEngine/Core/EntryPoint.h>

#include "StarEngine/Renderer/OrthographicCameraController.h"
#include "Sandbox2D.h"

#include "ExampleLayer.h"

class Sandbox : public StarEngine::Application
{
public:
	Sandbox()
	{
		//PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}

};

StarEngine::Application* StarEngine::CreateApplication()
{
	return new Sandbox();
}