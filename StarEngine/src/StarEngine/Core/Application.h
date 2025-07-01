#pragma once

#include "StarEngine/Core/Base.h"

#include "StarEngine/Core/Window.h"
#include "StarEngine/Core/LayerStack.h"
#include "StarEngine/Core/Thread.h"
#include "StarEngine/Events/Event.h"
#include "StarEngine/Events/ApplicationEvent.h"

#include "StarEngine/Core/Timestep.h"

#include "StarEngine/ImGui/ImGuiLayer.h"

#include "StarEngine/Renderer/DeviceManager.h"
#include "StarEngine/Renderer/RenderThread.h"
#include "StarEngine/Renderer/RendererConfig.h"

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
		std::string Name = "StarEngine";
		uint32_t WindowWidth = 1920, WindowHeight = 1080;
		bool Fullscreen = false;
		bool VSync = true;
		bool StartMaximized = true;
		bool Resizable = true;
		bool EnableImGui = true;
		bool ShowSplashScreen = false;
		RendererConfig RenderConfig;
		ThreadingPolicy CoreThreadingPolicy = ThreadingPolicy::MultiThreaded;
		std::filesystem::path IconPath;
		std::string WorkingDirectory;
	};

	class Application
	{
		public:
			Application(const ApplicationSpecification& specification);
			virtual ~Application();

			void OnEvent(Event& e);

			void Run();

			void PushLayer(Layer* layer);
			void PushOverlay(Layer* layer);

			Window& GetWindow() { return *m_Window; }

			void Close();

			ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

			static Application& Get() { return *s_Instance; }

			const ApplicationSpecification& GetSpecification() const { return m_Specification; }

			void SubmitToMainThread(const std::function<void()>& function);

			RenderThread& GetRenderThread() { return m_RenderThread; }
			
			static nvrhi::DeviceHandle GetGraphicsDeviceManager() { return Application::Get().GetWindow().GetDeviceManager(); }
		
			static std::thread::id GetMainThreadID();
			static bool IsMainThread();
	private:
			bool OnWindowClose(WindowCloseEvent& e);
			bool OnWindowResize(WindowResizeEvent& e);

			void ExecuteMainThreadQueue();
		private:
			ApplicationSpecification m_Specification;
			std::unique_ptr<Window> m_Window;
			ImGuiLayer* m_ImGuiLayer;
			bool m_Running = true;
			bool m_Minimized = false;
			LayerStack m_LayerStack;
			float m_LastFrameTime = 0.0f;

			static nvrhi::DeviceHandle s_GraphicsDevice; // Add this member to store the graphics device
			Ref<DeviceManager> m_GraphicsDeviceManager; // Add this member to store the device manager

			std::vector<std::function<void()>> m_MainThreadQueue;
			std::mutex m_MainThreadQueueMutex;

			RenderThread m_RenderThread;

			static std::thread::id s_MainThreadID;
		private:
				static Application* s_Instance;
				friend int ::main(int argc, char** argv);
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);
}


