#include "sepch.h"
#include "StarEngine/ImGui/ImGuiLayer.h"

// Include the ImGuiRenderer header file
#include "StarEngine/ImGui/ImGuiRenderer.h"

#include "StarEngine/Core/Application.h"

#include "StarEngine/Core/Input.h"

#include "StarEngine/Renderer/DeviceManager.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

// TEMPORARY
#include <GLFW/glfw3.h>

#include "ImGuizmo.h"

namespace StarEngine {

	void ImGuiLayer::OnAttach()
	{
		SE_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		float fontSize = 18.0f;// *2.0f;

		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(), true);
		m_ImGuiRenderer.Init(m_Device);

		InitPlatformInterface();
	}

	struct ImGuiViewportData {
		bool WindowOwned = false;
		std::unique_ptr<DeviceManager> DM;
	};

	static void ImGuiRenderer_CreateWindow(ImGuiViewport* viewport)
	{
#if 0
		ImGuiViewportData* data = IM_NEW(ImGuiViewportData)();
		viewport->RendererUserData = data;

		DeviceCreationParameters deviceParams;
		deviceParams.Decorated = m_Specification.Decorated;

		data->DM = std::make_unique <DeviceManager>();

		ImGuiPlatformIO& platform_IO = ImGui::GetPlatformIO();
		VkResult err = (VkResult)platform_IO.Platform_CreateVkSurface(viewport, (ImU64)v->Instance, (const void*)v-Allocator);
		check_vk_result(err);
#endif
	}

	void ImGuiLayer::InitPlatformInterface()
	{
		ImGuiPlatformIO& platform_IO = ImGui::GetPlatformIO();
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			IM_ASSERT(platform_IO.Platform_CreateVkSurface != NULL && "Platform needs to set up the CreateVkSurfaceHandler.");

		platform_IO.Renderer_CreateWindow = ImGuiRenderer_CreateWindow;
		//platform_IO.Renderer_DestroyWindow = ImGui_ImplVulkan_DestroyWindow;
		//platform_IO.Renderer_SetWindowSize = ImGui_ImplVulkan_SetWindowSize;
		//platform_IO.Renderer_RenderWindow = ImGui_ImplVulkan_RenderWindow;
		//platform_IO.Renderer_SwapBuffers = ImGui_ImplVulkan_SwapBuffers;
	}

	void ImGuiLayer::OnDetach()
	{
		SE_PROFILE_FUNCTION();

		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		SE_PROFILE_FUNCTION();

		ImGui::SetMouseCursor(Input::GetCursorMode() == CursorMode::Normal ? ImGuiMouseCursor_Arrow : ImGuiMouseCursor_None);

		m_ImGuiRenderer->UpdateFontTexture();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End()
	{
		SE_PROFILE_FUNCTION();

		ImGui::Render();

		m_ImGuiRenderer->Render(Application::GetGraphicsDeviceManager()->GetCurrentFramebuffer());

#if TODO
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#endif
	}

	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}
}
