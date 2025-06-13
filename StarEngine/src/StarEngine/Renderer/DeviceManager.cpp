#include "sepch.h"

#include "StarEngine/Renderer/DeviceManager.h"
#include "StarEngine/Core/Log.h"
#include <nvrhi/utils.h>

#include <glm/glm.hpp>

#include <cstdio>
#include <iomanip>
#include <thread>
#include <sstream>

#ifdef SE_WITH_DX11
#include <d3d11.h>
#endif

#ifdef SE_WITH_DX12
#include <d3d12.h>
#endif

#ifdef SE_PLATFORM_WINDOWS
#include <ShellScalingApi.h>
#pragma comment(lib, "shcore.lib")
#endif

#if defined(SE_PLATFORM_WINDOWS) && SE_FORCE_DISCRETE_GPU
extern "C"
{
	// Declaring this symbol makes the OS run the app on the discrete GPU on NVIDIA Optimus laptops by default
	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	// Same as above, for laptops with AMD GPUs
	__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 1;
}
#endif

namespace fmt {
	namespace v11 {
		namespace detail {

			// Define the struct type_is_unformattable_for
			template <typename T, typename Char>
			struct type_is_unformattable_for {
				static constexpr bool value = false; // Default implementation
			};

		} // namespace detail
	} // namespace v11
} // namespace fmt

namespace StarEngine {

	// The joystick interface in glfw is not per-window like the keys, mouse, etc. The joystick callbacks
	// don't take a window arg. So glfw's model is a global joystick shared by all windows. Hence, the equivalent 
	// is a singleton class that all DeviceManager instances can use.

	static const struct
	{
		nvrhi::Format format;
		uint32_t redBits;
		uint32_t greenBits;
		uint32_t blueBits;
		uint32_t alphaBits;
		uint32_t depthBits;
		uint32_t stencilBits;
	} formatInfo[] = {
		{ nvrhi::Format::UNKNOWN,            0,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::R8_UINT,            8,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::RG8_UINT,           8,  8,  0,  0,  0,  0, },
		{ nvrhi::Format::RG8_UNORM,          8,  8,  0,  0,  0,  0, },
		{ nvrhi::Format::R16_UINT,          16,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::R16_UNORM,         16,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::R16_FLOAT,         16,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::RGBA8_UNORM,        8,  8,  8,  8,  0,  0, },
		{ nvrhi::Format::RGBA8_SNORM,        8,  8,  8,  8,  0,  0, },
		{ nvrhi::Format::BGRA8_UNORM,        8,  8,  8,  8,  0,  0, },
		{ nvrhi::Format::SRGBA8_UNORM,       8,  8,  8,  8,  0,  0, },
		{ nvrhi::Format::SBGRA8_UNORM,       8,  8,  8,  8,  0,  0, },
		{ nvrhi::Format::R10G10B10A2_UNORM, 10, 10, 10,  2,  0,  0, },
		{ nvrhi::Format::R11G11B10_FLOAT,   11, 11, 10,  0,  0,  0, },
		{ nvrhi::Format::RG16_UINT,         16, 16,  0,  0,  0,  0, },
		{ nvrhi::Format::RG16_FLOAT,        16, 16,  0,  0,  0,  0, },
		{ nvrhi::Format::R32_UINT,          32,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::R32_FLOAT,         32,  0,  0,  0,  0,  0, },
		{ nvrhi::Format::RGBA16_FLOAT,      16, 16, 16, 16,  0,  0, },
		{ nvrhi::Format::RGBA16_UNORM,      16, 16, 16, 16,  0,  0, },
		{ nvrhi::Format::RGBA16_SNORM,      16, 16, 16, 16,  0,  0, },
		{ nvrhi::Format::RG32_UINT,         32, 32,  0,  0,  0,  0, },
		{ nvrhi::Format::RG32_FLOAT,        32, 32,  0,  0,  0,  0, },
		{ nvrhi::Format::RGB32_UINT,        32, 32, 32,  0,  0,  0, },
		{ nvrhi::Format::RGB32_FLOAT,       32, 32, 32,  0,  0,  0, },
		{ nvrhi::Format::RGBA32_UINT,       32, 32, 32, 32,  0,  0, },
		{ nvrhi::Format::RGBA32_FLOAT,      32, 32, 32, 32,  0,  0, },
	};

	bool DeviceManager::CreateInstance(const InstanceParameters& params)
	{
		if (m_InstanceCreated)
			return true;


		static_cast<InstanceParameters&>(m_DeviceParams) = params;

		if (!params.headlessDevice)
		{
	#ifdef SE_PLATFORM_WINDOWS
			if (!params.enablePerMonitorDPI)
			{
				// glfwInit enables the maximum supported level of DPI awareness unconditionally.
				// If the app doesn't need it, we have to call this function before glfwInit to override that behavior.
				SetProcessDpiAwareness(PROCESS_DPI_UNAWARE);
			}
	#endif

			if (!glfwInit())
				return false;
		}

		m_InstanceCreated = CreateInstanceInternal();
		return m_InstanceCreated;
	}

	bool DeviceManager::CreateHeadlessDevice(const DeviceCreationParameters& params)
	{
		m_DeviceParams = params;
		m_DeviceParams.headlessDevice = true;

		if (!CreateInstance(m_DeviceParams))
			return false;

		return CreateDevice();
	}

	bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle)
	{
		m_DeviceParams = params;
		m_DeviceParams.headlessDevice = false;
		m_RequestedVSync = params.vsyncEnabled;

		if (!CreateInstance(m_DeviceParams))
			return false;

		//glfwSetErrorCallback(ErrorCallback_GLFW);

		glfwDefaultWindowHints();

		bool foundFormat = false;
		for (const auto& info : formatInfo)
		{
			if (info.format == params.swapChainFormat)
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

		glfwWindowHint(GLFW_SAMPLES, params.swapChainSampleCount);
		glfwWindowHint(GLFW_REFRESH_RATE, params.refreshRate);
		glfwWindowHint(GLFW_SCALE_TO_MONITOR, params.resizeWindowWithDisplayScale);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);   // Ignored for fullscreen

		if (params.startBorderless)
		{
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // Borderless window
		}

		m_Window = glfwCreateWindow(params.backBufferWidth, params.backBufferHeight,
			windowTitle ? windowTitle : "",
			params.startFullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);

		if (m_Window == nullptr)
		{
			return false;
		}

		if (params.startFullscreen)
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

		if (windowTitle)
			m_WindowTitle = windowTitle;

		glfwSetWindowUserPointer(m_Window, this);

		if (params.windowPosX != -1 && params.windowPosY != -1)
		{
			glfwSetWindowPos(m_Window, params.windowPosX, params.windowPosY);
		}

		// If there are multiple device managers, then this would be called by each one which isn't necessary
		// but should not hurt.

		if (!CreateDevice())
			return false;

		if (!CreateSwapChain())
			return false;

		glfwShowWindow(m_Window);

		if (m_DeviceParams.startMaximized)
		{
			glfwMaximizeWindow(m_Window);
		}

		// reset the back buffer size state to enforce a resize event
		m_DeviceParams.backBufferWidth = 0;
		m_DeviceParams.backBufferHeight = 0;

		UpdateWindowSize();

		return true;
	}

	void DeviceManager::AddRenderPassToFront(IRenderPass* pRenderPass)
	{
		m_vRenderPasses.remove(pRenderPass);
		m_vRenderPasses.push_front(pRenderPass);

		pRenderPass->BackBufferResizing();
		pRenderPass->BackBufferResized(
			m_DeviceParams.backBufferWidth,
			m_DeviceParams.backBufferHeight,
			m_DeviceParams.swapChainSampleCount);
	}

	void DeviceManager::AddRenderPassToBack(IRenderPass* pRenderPass)
	{
		m_vRenderPasses.remove(pRenderPass);
		m_vRenderPasses.push_back(pRenderPass);

		pRenderPass->BackBufferResizing();
		pRenderPass->BackBufferResized(
			m_DeviceParams.backBufferWidth,
			m_DeviceParams.backBufferHeight,
			m_DeviceParams.swapChainSampleCount);
	}

	void DeviceManager::RemoveRenderPass(IRenderPass* pRenderPass)
	{
		m_vRenderPasses.remove(pRenderPass);
	}

	void DeviceManager::BackBufferResizing()
	{
		m_SwapChainFramebuffers.clear();

		for (auto it : m_vRenderPasses)
		{
			it->BackBufferResizing();
		}
	}

	void DeviceManager::BackBufferResized()
	{
		for (auto it : m_vRenderPasses)
		{
			it->BackBufferResized(m_DeviceParams.backBufferWidth,
				m_DeviceParams.backBufferHeight,
				m_DeviceParams.swapChainSampleCount);
		}

		uint32_t backBufferCount = GetBackBufferCount();
		m_SwapChainFramebuffers.resize(backBufferCount);
		for (uint32_t index = 0; index < backBufferCount; index++)
		{
			m_SwapChainFramebuffers[index] = GetDevice()->createFramebuffer(
				nvrhi::FramebufferDesc().addColorAttachment(GetBackBuffer(index)));
		}
	}

	void DeviceManager::DisplayScaleChanged()
	{
		for (auto it : m_vRenderPasses)
		{
			it->DisplayScaleChanged(m_DPIScaleFactorX, m_DPIScaleFactorY);
		}
	}

	void DeviceManager::Animate(double elapsedTime)
	{
		for (auto it : m_vRenderPasses)
		{
			it->Animate(float(elapsedTime));
			it->SetLatewarpOptions();
		}
	}

	void DeviceManager::Render()
	{
		nvrhi::IFramebuffer* framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];

		for (auto it : m_vRenderPasses)
		{
			it->Render(framebuffer);
		}
	}

	void DeviceManager::UpdateAverageFrameTime(double elapsedTime)
	{
		m_FrameTimeSum += elapsedTime;
		m_NumberOfAccumulatedFrames += 1;

		if (m_FrameTimeSum > m_AverageTimeUpdateInterval && m_NumberOfAccumulatedFrames > 0)
		{
			m_AverageFrameTime = m_FrameTimeSum / double(m_NumberOfAccumulatedFrames);
			m_NumberOfAccumulatedFrames = 0;
			m_FrameTimeSum = 0.0;
		}
	}

	bool DeviceManager::ShouldRenderUnfocused() const
	{
		for (auto it = m_vRenderPasses.crbegin(); it != m_vRenderPasses.crend(); it++)
		{
			bool ret = (*it)->ShouldRenderUnfocused();
			if (ret)
				return true;
		}

		return false;
	}

	void DeviceManager::RunMessageLoop()
	{
		m_PreviousFrameTimestamp = glfwGetTime();

		while (!glfwWindowShouldClose(m_Window))
		{
			if (m_callbacks.beforeFrame) m_callbacks.beforeFrame(*this, m_FrameIndex);
			glfwPollEvents();
			UpdateWindowSize();
			bool presentSuccess = AnimateRenderPresent();
			if (!presentSuccess)
			{
				break;
			}
		}

		bool waitSuccess = GetDevice()->waitForIdle();
	}

	bool DeviceManager::AnimateRenderPresent()
	{
		double curTime = glfwGetTime();
		double elapsedTime = curTime - m_PreviousFrameTimestamp;

		if (m_windowVisible && (m_windowIsInFocus || ShouldRenderUnfocused()))
		{
			if (m_PrevDPIScaleFactorX != m_DPIScaleFactorX || m_PrevDPIScaleFactorY != m_DPIScaleFactorY)
			{
				DisplayScaleChanged();
				m_PrevDPIScaleFactorX = m_DPIScaleFactorX;
				m_PrevDPIScaleFactorY = m_DPIScaleFactorY;
			}

			if (m_callbacks.beforeAnimate) m_callbacks.beforeAnimate(*this, m_FrameIndex);
			Animate(elapsedTime);
			if (m_callbacks.afterAnimate) m_callbacks.afterAnimate(*this, m_FrameIndex);

			// normal rendering           : A0    R0 P0 A1 R1 P1
			// m_SkipRenderOnFirstFrame on: A0 A1 R0 P0 A2 R1 P1
			// m_SkipRenderOnFirstFrame simulates multi-threaded rendering frame indices, m_FrameIndex becomes the simulation index
			// while the local variable below becomes the render/present index, which will be different only if m_SkipRenderOnFirstFrame is set
			if (m_FrameIndex > 0 || !m_SkipRenderOnFirstFrame)
			{
				if (BeginFrame())
				{
					// first time entering this loop, m_FrameIndex is 1 for m_SkipRenderOnFirstFrame, 0 otherwise;
					uint32_t frameIndex = m_FrameIndex;

					if (m_SkipRenderOnFirstFrame)
					{
						frameIndex--;
					}

					if (m_callbacks.beforeRender) m_callbacks.beforeRender(*this, frameIndex);
					Render();
					if (m_callbacks.afterRender) m_callbacks.afterRender(*this, frameIndex);
					if (m_callbacks.beforePresent) m_callbacks.beforePresent(*this, frameIndex);
					bool presentSuccess = Present();
					if (m_callbacks.afterPresent) m_callbacks.afterPresent(*this, frameIndex);
					if (!presentSuccess)
					{
						return false;
					}
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(0));

		GetDevice()->runGarbageCollection();

		UpdateAverageFrameTime(elapsedTime);
		m_PreviousFrameTimestamp = curTime;

		++m_FrameIndex;
		return true;
	}

	void DeviceManager::GetWindowDimensions(int& width, int& height)
	{
		width = m_DeviceParams.backBufferWidth;
		height = m_DeviceParams.backBufferHeight;
	}

	const DeviceCreationParameters& DeviceManager::GetDeviceParams()
	{
		return m_DeviceParams;
	}

	DeviceManager::DeviceManager()
	{

	}

	void DeviceManager::Shutdown()
	{
		m_SwapChainFramebuffers.clear();

		DestroyDeviceAndSwapChain();

		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}

		glfwTerminate();

		m_InstanceCreated = false;
	}

	nvrhi::IFramebuffer* DeviceManager::GetCurrentFramebuffer()
	{
		return GetFramebuffer(GetCurrentBackBufferIndex());
	}

	nvrhi::IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
	{
		if (index < m_SwapChainFramebuffers.size())
			return m_SwapChainFramebuffers[index];

		return nullptr;
	}

	void DeviceManager::SetWindowTitle(const char* title)
	{
		assert(title);
		if (m_WindowTitle == title)
			return;

		glfwSetWindowTitle(m_Window, title);

		m_WindowTitle = title;
	}

	void DeviceManager::SetInformativeWindowTitle(const char* applicationName, bool includeFramerate, const char* extraInfo)
	{
		std::stringstream ss;
		ss << applicationName;
		ss << " (" << nvrhi::utils::GraphicsAPIToString(GetDevice()->getGraphicsAPI());

		if (m_DeviceParams.enableDebugRuntime)
		{
			if (GetGraphicsAPI() == nvrhi::GraphicsAPI::VULKAN)
				ss << ", VulkanValidationLayer";
			else
				ss << ", DebugRuntime";
		}

		if (m_DeviceParams.enableNvrhiValidationLayer)
		{
			ss << ", NvrhiValidationLayer";
		}

		ss << ")";

		double frameTime = GetAverageFrameTimeSeconds();
		if (includeFramerate && frameTime > 0)
		{
			double const fps = 1.0 / frameTime;
			int const precision = (fps <= 20.0) ? 1 : 0;
			ss << " - " << std::fixed << std::setprecision(precision) << fps << " FPS ";
		}

		if (extraInfo)
			ss << extraInfo;

		SetWindowTitle(ss.str().c_str());
	}

	const char* DeviceManager::GetWindowTitle()
	{
		return m_WindowTitle.c_str();
	}

	DeviceManager* DeviceManager::Create(nvrhi::GraphicsAPI api)
	{
		switch (api)
		{
	#ifdef SE_WITH_DX11
		case nvrhi::GraphicsAPI::D3D11:
			return CreateD3D11();
	#endif
	#ifdef SE_WITH_DX12
		case nvrhi::GraphicsAPI::D3D12:
			return CreateD3D12();
	#endif
	#ifdef SE_WITH_VULKAN
		case nvrhi::GraphicsAPI::VULKAN:
			return CreateVK();
	#endif
		default:
			SE_CORE_ERROR("DeviceManager::Create: Unsupported Graphics API (%d)", api);
			return nullptr;
		}
	}

	DefaultMessageCallback& DefaultMessageCallback::GetInstance()
	{
		static DefaultMessageCallback Instance;
		return Instance;
	}

	void DefaultMessageCallback::message(nvrhi::MessageSeverity severity, const char* messageText)
	{
		StarEngine::LogSeverity starSeverity = StarEngine::LogSeverity::Info;
		switch (severity)
		{
		case nvrhi::MessageSeverity::Info:
			starSeverity = StarEngine::LogSeverity::Info;
			break;
		case nvrhi::MessageSeverity::Warning:
			starSeverity = StarEngine::LogSeverity::Warn;
			break;
		case nvrhi::MessageSeverity::Error:
			starSeverity = StarEngine::LogSeverity::Error;
			break;
		case nvrhi::MessageSeverity::Fatal:
			starSeverity = StarEngine::LogSeverity::Critical;
			break;
		}

		Log::LogMessage(starSeverity, "%s", messageText);
	}

}
