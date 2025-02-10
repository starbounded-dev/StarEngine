#pragma once

#include "StarStudio/Core/Base.h"

#include "StarStudio/Core/Window.h"
#include "StarStudio/Core/LayerStack.h"
#include "StarStudio/Events/Event.h"
#include "StarStudio/Events/ApplicationEvent.h"

#include "StarStudio/Core/Timestep.h"

#include "StarStudio/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace StarStudio
{
	class Application
	{
		public:
			Application();
			virtual ~Application();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
			Window& GetWindow() { return *m_Window; }
			static Application& Get() { return *s_Instance; }
		private:
			void Run();

			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);
		private:
			std::unique_ptr<Window> m_Window;
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
	Application* CreateApplication();
}


