#pragma once

#include "Core.h"

#include "Window.h"
#include "StarStudio/LayerStack.h"
#include "StarStudio/Events/Event.h"
#include "StarStudio/Events/ApplicationEvent.h"

#include "StarStudio/Core/Timestep.h"

#include "StarStudio/ImGui/ImGuiLayer.h"

namespace StarStudio
{
	class Application
	{
		public:
			Application();
			virtual ~Application();

			void Run();

			void OnEvent(Event& e);

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);
			inline Window& GetWindow() { return *m_Window; }
			inline static Application& Get() { return *s_Instance; }
		private:
			bool OnWindowClose(WindowCloseEvent& e);

			std::unique_ptr<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;

			bool m_Running = true;
			LayerStack m_LayerStack;

			float m_LastFrameTime = 0.0f;
	private:
			static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}


