#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_internal.h"


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{

}

void Sandbox2D::OnAttach()
{
	SE_PROFILE_FUNCTION();

	m_CheckerboardTexture = StarEngine::Texture2D::Create("assets/textures/Checkerboard.png");

	StarEngine::FramebufferSpecification fbSpec;
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = StarEngine::Framebuffer::Create(fbSpec);
}

void Sandbox2D::OnDetach()
{
	SE_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(StarEngine::Timestep ts)
{
	SE_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	StarEngine::Renderer2D::ResetStats();
	{
		SE_PROFILE_SCOPE("Renderer Prep");
		m_Framebuffer->Bind();
		StarEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		StarEngine::RenderCommand::Clear();
	}
	
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		SE_PROFILE_SCOPE("Renderer Draw");

		StarEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		StarEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		StarEngine::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, rotation, { 0.8f, 0.2f, 0.3f, 1.0f });
		StarEngine::Renderer2D::EndScene();

		m_Framebuffer->Unbind();
	}
}

void Sandbox2D::OnImGuiRender()
{
	SE_PROFILE_FUNCTION();

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;

	if (true) {
		static bool dockspaceOpen = false;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit")) StarEngine::Application::Get().Close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		//Camera Info
		ImGui::Begin("Camera Info");

		auto stats = StarEngine::Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::Separator();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

		if (ImGui::Checkbox("VSync", &m_VSync)) {
			// Toggle VSync when the checkbox is clicked
			StarEngine::Application::Get().GetWindow().SetVSync(m_VSync);
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGui::Image(textureID, ImVec2{ 1280, 720 });

		ImGui::End();

		ImGui::End();
	}
}

void Sandbox2D::OnEvent(StarEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
