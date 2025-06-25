#pragma once

#include "StarEngine/Core/Core.h"
#include "StarEngine/Events/Event.h"

#include <sstream>

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

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		DeviceManager* GetDeviceManager() const;

		static std::unique_ptr<Window> Create(const WindowSpecification& props = WindowSpecification());
	public:
		static float s_HighDPIScaleFactor;
	};

}
