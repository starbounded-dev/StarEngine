#pragma once

#include "Core.h"

#include "Window.h"
#include "StarStudio/LayerStack.h"
#include "StarStudio/Events/Event.h"
#include "StarStudio/Events/ApplicationEvent.h"

#include "StarStudio/ImGui/ImGuiLayer.h"

#include "StarStudio/Renderer/Shader.h"
#include "StarStudio/Renderer/Buffer.h"
#include "StarStudio/Renderer/VertexArray.h"

namespace StarStudio
{
	class STARSTUDIO_API Application
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

			std::shared_ptr<Shader> m_Shader;
			std::shared_ptr<VertexArray> m_VertexArray;

			std::shared_ptr<Shader> m_BlueShader;
			std::shared_ptr<VertexArray> m_SquareVA;

	private:
			static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}


