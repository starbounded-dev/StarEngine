#include "sepch.h"
#include "StarEngine/Core/Window.h"

#include "StarEngine/Renderer/DeviceManager.h"

#ifdef SE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif  

#include "stb_image.h"
#include <nvrhi/nvrhi.h>

namespace StarEngine
{
	static void GLFWErrorCallback(int error, const char* description)
	{
		SE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowSpecification& specification)
	{
		return new Window(specification);
	}

	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification)
	{
		
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::Init()
	{
		m_Data.Title = m_Specification.Title;
		m_Data.Width = m_Specification.Width;
		m_Data.Height = m_Specification.Height;

		DeviceCreationParameters deviceParams;
		deviceParams.decorated = m_Specification.Decorated;
		deviceParams.swapChainBufferCount = 2;
		deviceParams.enableRayTracingExtensions = false;
		deviceParams.backBufferWidth = 1920;
		deviceParams.backBufferHeight = 1080;
		deviceParams.vsyncEnabled = false;
		deviceParams.enableDebugRuntime = true;

		nvrhi::GraphicsAPI api = nvrhi::GraphicsAPI::Vulkan;

		m_DeviceManager = DeviceManager::Create(api);
		m_DeviceManager->SetWindowContext(this);

		if (!m_DeviceManager->CreateWindowDeviceAndSwapChain(deviceParams, m_Specification.Title.c_str()))
		{
			SE_CORE_ERROR("Cannot initialize a {} graphics device.", (uint8_t)api);
			return;
		}
		else
		{
			SE_CORE_INFO("Successfully created a {} graphics device.", (uint8_t)api);
		}

		bool rayQuerySupported = m_DeviceManager->GetDevice()->queryFeatureSupport(nvrhi::Feature::RayQuery);

		if (!rayQuerySupported)
		{
			SE_CORE_ERROR("Ray Query is not supported by the device. Please check your device capabilities.");
			return;
		}
		else {
			SE_CORE_INFO("Ray Query is supported by the device.");
		}

		m_Window = m_DeviceManager->GetWindow();

		glfwSetErrorCallback(GLFWErrorCallback);

		glfwDefaultWindowHints();

		bool foundFormat = false;
		for (const auto& info : formatInfo)
		{
			if (info.format == deviceParams.swapChainFormat)
			{
				glfwWindowHint(GLFW_RED_BITS, info.redBits);
				glfwWindowHint(GLFW_GREEN_BITS, info.greenBits);
				glfwWindowHint(GLFW_BLUE_BITS, info.blueBits);
				glfwWindowHint(GLFW_ALPHA_BITS, info.alphaBits);
				glfwWindowHint(GLFW_DEPTH_BITS, info.depthBits);
				glfwWindowHint(GLFW_STENCIL_BITS, info.stencilBits);
				foundFormat = true;
				break;
			}
		}

		assert(foundFormat);

		glfwWindowHint(GLFW_SAMPLES, deviceParams.swapChainSampleCount);
		glfwWindowHint(GLFW_REFRESH_RATE, deviceParams.refreshRate);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, deviceParams.resizeWindowWithDisplayScale);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Ignored for fullscreen

		if (!m_Specification.Decorated)
		{
#ifdef SE_PLATFORM_WINDOWS
			glfwWindowHint(GLFW_TITLEBAR, GLFW_FALSE); // Borderless window
#else
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
#endif
		}

		m_Window = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Data.Title.c_str(), m_Specification.Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

		glfwSetWindowUserPointer(m_Window, this);

		if (m_Window == nullptr)
		{
			// return false;
		}

		if (deviceParams.startFullscreen)
		{
			glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
				m_DeviceParams.backBufferWidth, m_DeviceParams.backBufferHeight, m_DeviceParams.refreshRate);
		}
		else
		{
			int fbWidth = 0, fbHeight = 0;
			glfwGetFramebufferSize(m_Window, &fbWidth, &fbHeight);
			m_DeviceParams.backBufferWidth = fbWidth;
			m_DeviceParams.backBufferHeight = fbHeight;
		}

		if (deviceParams.windowPosX != -1 && deviceParams.windowPosY != -1)
		{
			glfwSetWindowPos(m_Window, deviceParams.windowPosX, deviceParams.windowPosY);
		}

		// Set Icon
		{
			GLFWimage icon;
			int channels;

			bool useEmbedded = m_Specification.IconPath.empty();

			if (!useEmbedded)
			{
				std::string iconPathStr = m_Specification.IconPath.string();
				icon.pixels = stbi_load(iconPathStr.c_str(), &icon.width, &icon.height, &channels, 4);
				if (icon.pixels)
				{
					glfwSetWindowIcon(m_Window, 1, &icon);
					stbi_image_free(icon.pixels);
				}
				else
				{
					useEmbedded = true;
				}
			}

			if (useEmbedded)
			{
				icon.pixels = stbi_load_from_memory(g_StarIconPNG, sizeof(g_StarIconPNG), &icon.width, &icon.height, &channels, 4);
				glfwSetWindowIcon(m_Window, 1, &icon);
				stbi_image_free(icon.pixels);
			}
		}
	}

	DeviceManager* Window::GetDeviceManager() const
	{
		return nullptr;
	}
}
