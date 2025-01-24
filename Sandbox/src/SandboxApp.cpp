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
		//SS_INFO("ExampleLayer::Update");

		if (StarStudio::Input::IsKeyPressed(SS_KEY_TAB))
			SS_TRACE("Tab key is pressed!");
	}
	void OnEvent(StarStudio::Event& event) override
	{
		if (event.GetEventType() == StarStudio::EventType::KeyPressed)
		{
			StarStudio::KeyPressedEvent& e = (StarStudio::KeyPressedEvent&)event;
			if (e.GetKeyCode() == SS_KEY_TAB)
				SS_TRACE("Tab key is pressed (event)!");
			SS_TRACE("{0}", (char)e.GetKeyCode());
		}
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