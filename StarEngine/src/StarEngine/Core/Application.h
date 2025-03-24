#pragma once

#include "StarEngine/Core/Base.h"

#include "StarEngine/Core/Window.h"
#include "StarEngine/Core/LayerStack.h"
#include "StarEngine/Events/Event.h"
#include "StarEngine/Events/ApplicationEvent.h"

#include "StarEngine/Core/Timestep.h"

#include "StarEngine/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace StarEngine
{
	struct ApplicationCommandLineArgs
	{
		int Count = 0;
		char** Args = nullptr;

		const char* operator[](int index) const
		{
			SE_CORE_ASSERT(index < Count);
			return Args[index];
		}
	};

	struct ApplicationSpecification
	{
		std::string Name = "StarEngine Application";
		std::string WorkingDirectory;
		ApplicationCommandLineArgs CommandLineArgs;
	};

	class Application
	{
		public:
			Application(const ApplicationSpecification& specification);
			virtual ~Application();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);

			Window& GetWindow() { return *m_Window; }

			void Close();

			ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

			static Application& Get() { return *s_Instance; }

			const ApplicationSpecification& GetSpecification() const { return m_Specification; }
		private:
			void Run();

			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);
		private:
			ApplicationSpecification m_Specification;
			Scope<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;
			float m_LastFrameTime = 0.0f;
		private:
				static Application* s_Instance;
				friend int ::main(int argc, char** argv);
	};

	// To be defined in CLIENT
	Application* CreateApplication(ApplicationCommandLineArgs args);
}


