#include "sepch.h"
#include "StarEngine/Core/Window.h"

#include "StarEngine/Renderer/DeviceManager.h"

#include "StarEngine/Core/Input.h"

#include "StarEngine/Events/ApplicationEvent.h"
#include "StarEngine/Events/MouseEvent.h"
#include "StarEngine/Events/KeyEvent.h"

#include "stb_image.h"
#include <nvrhi/nvrhi.h>

namespace StarEngine
{
	float Window::s_HighDPIScaleFactor = 1.0f;

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		SE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	static bool s_GLFWInitialized = false;

	std::unique_ptr<Window> Window::Create(const WindowSpecification& specification)
	{
		return std::make_unique<Window>(specification);
	}

	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification)
	{
		SE_PROFILE_FUNCTION();

		Init(specification);
	}

	Window::~Window()
	{
		SE_PROFILE_FUNCTION();

		Shutdown();
	}

	void Window::OnUpdate()
	{
		SE_PROFILE_FUNCTION();

		glfwPollEvents();
		m_Context->SwapBuffers();
	}

	/*
	void Window::Init(specification)
	{
		m_Specification.Title = m_Specification.Title;
		m_Specification.Width = m_Specification.Width;
		m_Specification.Height = m_Specification.Height;

		DeviceCreationParameters deviceParams;
		deviceParams.decorated = m_Specification.Decorated;
		deviceParams.swapChainBufferCount = 2;
		deviceParams.enableRayTracingExtensions = false;
		deviceParams.backBufferWidth = 1920;
		deviceParams.backBufferHeight = 1080;
		deviceParams.vsyncEnabled = false;
		deviceParams.enableDebugRuntime = true;

		nvrhi::GraphicsAPI api = nvrhi::GraphicsAPI::VULKAN;

		m_DeviceManager = DeviceManager::Create(api);
		m_DeviceManager->SetWindowContext(this);

		if (!m_DeviceManager->CreateDeviceAndSwapChain(deviceParams, m_Specification.Title.c_str()))
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
		for (const auto& info : FormatInfo)
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

		m_Window = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Title.c_str(), m_Specification.Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

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
		/*
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
		
	}*/
	
	void Window::Init(WindowSpecification& spec)
	{
		SE_PROFILE_FUNCTION();

		m_Specification.Title = spec.Title;
		m_Specification.Width = spec.Width;
		m_Specification.Height = spec.Height;

		SE_CORE_INFO("Creating window {0} ({1}, {2})", spec.Title, spec.Width, spec.Height);

		if (s_GLFWWindowCount == 0)
		{
			SE_PROFILE_SCOPE("glfwInit");
			int success = glfwInit();
			SE_CORE_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		{
			SE_PROFILE_SCOPE("glfwCreateWindow");

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			float xscale, yscale;
			glfwGetMonitorContentScale(monitor, &xscale, &yscale);

			if (xscale > 1.0f || yscale > 1.0f)
			{
				s_HighDPIScaleFactor = yscale;
				glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
			}

#if defined(SE_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

			m_Window = glfwCreateWindow((int)spec.Width, (int)spec.Height, m_Specification.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;
		}

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Specification);
		SetVSync(true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, true);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int pathCount, const char* paths[])
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				std::vector<std::filesystem::path> filepaths(pathCount);
				for (int i = 0; i < pathCount; i++)
					filepaths[i] = paths[i];

				WindowDropEvent event(std::move(filepaths));
				data.EventCallback(event);
			});
	}

	void Window::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::SetVSync(bool enabled)
	{
		SE_PROFILE_FUNCTION();

		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Specification.VSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return m_Specification.VSync;
	}

	DeviceManager* Window::GetDeviceManager()
	{
		m_DeviceManager->GetDevice();
	}
}
