#pragma once

#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Window.h"

#include "StarEngine/Events/Event.h"
#include "StarEngine/Renderer/GraphicsContext.h"

#include <sstream>

#include <GLFW/glfw3.h>

#include <nvrhi/nvrhi.h>

namespace StarEngine {

	struct WindowSpecification
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowSpecification(const std::string& title = "StarEngine",
			uint32_t width = 1600,
			uint32_t height = 900)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		Window(const WindowSpecification& spec);
		~Window();

		void OnUpdate();

		unsigned int GetWidth() const { return m_Specification.Width; }
		unsigned int GetHeight() const { return m_Specification.Height; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) { m_Specification.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool IsVSync() const;

		void* GetNativeWindow() const { return m_Window; }

		DeviceManager* GetDeviceManager();

		static std::unique_ptr<Window> Create(const WindowSpecification& spec = WindowSpecification());
	public:
		static float s_HighDPIScaleFactor;
	private:
		void Init(WindowSpecification& spec);
		void Shutdown();
	private:
		GLFWwindow* m_Window;
		std::unique_ptr<GraphicsContext> m_Context;
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Specification;

		Ref<DeviceManager> m_DeviceManager;
	};

}
