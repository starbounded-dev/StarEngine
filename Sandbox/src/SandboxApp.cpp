#include <StarStudio.h>
#include <StarStudio/Core/EntryPoint.h>

#include "StarStudio/Renderer/OrthographicCameraController.h"
#include "Sandbox2D.h"

#include "ExampleLayer.h"

class Sandbox : public StarStudio::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}

};

StarStudio::Application* StarStudio::CreateApplication()
{
	return new Sandbox();
}