#include <StarStudio.h>

class ExampleLayer : public StarStudio::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}
	void OnUpdate() override
	{

	}
	void OnEvent(StarStudio::Event& event) override
	{
		SS_TRACE("{0}", event);
	}
};

class Sandbox : public StarStudio::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
		PushOverlay(new StarStudio::ImGuiLayer());
	}
	~Sandbox() {

	}
};

StarStudio::Application* StarStudio::CreateApplication() {
	return new Sandbox();
}