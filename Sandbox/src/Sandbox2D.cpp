#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_internal.h"

static const uint32_t s_MapWidth = 15;
static const char* s_MapTiles = 
	"WWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWW"
	"WWWWWDDDWWWWWWW"
	"WWWDDDDDDDWWWWW"
	"WWDDDDDDDDDWWWW"
	"WWWDDWWDDDWWWWW"
	"WWDDDWWDDDDWWWW"
	"WWDDDWWDDDDWWWW"
	"WWWDDDWDDDDWWWW"
	"WWWDDDDDDDWWWWW"
	"WWDDDDDDDDWWWWW"
	"WWDDDDDDDDDWWWW"
	"WWWDDDDDDDWWWWW"
	"WWWWWWWWWWWWWWW"
	"WWWWWWWWWWWWWWW"
;


Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{

}

void Sandbox2D::OnAttach()
{
	SE_PROFILE_FUNCTION();

	m_CheckerboardTexture = StarEngine::Texture2D::Create("assets/textures/Checkerboard.png");
	m_SpriteSheet = StarEngine::Texture2D::Create("assets/game/textures/RPGpack_sheet_2X.png");

	m_TextureStairs = StarEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 7, 6 }, { 128, 128 }, { 1, 1 });
	m_TextureBarrel = StarEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 8, 2 }, { 128, 128 }, { 1, 1 });
	m_TextureTree = StarEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 4, 1 }, { 128, 128 }, { 1, 2 });

	m_MapWidth = s_MapWidth;
	m_MapHeight = strlen(s_MapTiles) / s_MapWidth;

	s_TextureMap['W'] = StarEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 11, 11 }, { 128, 128 }, { 1, 1 });
	s_TextureMap['D'] = StarEngine::SubTexture2D::CreateFromCoords(m_SpriteSheet, { 6, 11 }, { 128, 128 }, { 1, 1 });

	m_Particle.ColorBegin = { 254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f };
	m_Particle.ColorEnd = { 254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f };
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 0.6f;
	m_Particle.Velocity = { 0.0f, 0.0f };
	m_Particle.VelocityVariation = { 3.0f, 1.0f };
	m_Particle.Position = { 0.0f, 0.0f };
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
		StarEngine::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		StarEngine::RenderCommand::Clear();
	}
	
	{ 
		SE_PROFILE_SCOPE("Renderer Draw"); 
	
	if (StarEngine::Input::IsMouseButtonPressed(SE_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = StarEngine::Input::GetMousePosition();
		auto width = StarEngine::Application::Get().GetWindow().GetWidth();
		auto height = StarEngine::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = { x + pos.x, y + pos.y };
		for (int i = 0; i < 5; i++)
			m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
	}
}

void Sandbox2D::OnImGuiRender()
{
	SE_PROFILE_FUNCTION();

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;

	static bool dockspaceOpen = true;
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

	ImGui::Image(m_CheckerboardTexture->GetRendererID(), ImVec2{ 256.0f, 256.0f });

	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);
	ImGui::End();


	ImGui::End();
}

void Sandbox2D::OnEvent(StarEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
