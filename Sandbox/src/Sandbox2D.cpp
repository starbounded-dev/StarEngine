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
	"WWDDDWWDDDDWWWW"
	"WWWDDWWDDDWWWWW"
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
	/*
	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		SE_PROFILE_SCOPE("Renderer Draw");
		StarEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		StarEngine::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, glm::radians(-45.0f), { 0.8f, 0.2f, 0.3f, 1.0f });
		StarEngine::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		StarEngine::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		StarEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		StarEngine::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, glm::radians(rotation), m_CheckerboardTexture, 20.0f);
		StarEngine::Renderer2D::EndScene();


		StarEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				StarEngine::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		StarEngine::Renderer2D::EndScene();
	}
	*/
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

	StarEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());

	for (uint32_t y = 0; y < s_MapWidth; y++)
	{
		for (uint32_t x = 0; x < s_MapWidth; x++)
		{
			char tileType = s_MapTiles[x + y * s_MapWidth];
			StarEngine::Ref<StarEngine::SubTexture2D> texture;
			if (s_TextureMap.find(tileType) != s_TextureMap.end())
				texture = s_TextureMap[tileType];
			else
				texture = m_TextureStairs;
			StarEngine::Renderer2D::DrawQuad({ x - s_MapWidth / 2.0f, s_MapWidth / 2.0f - y }, { 1.0f, 1.0f }, texture);
		}
	}

	StarEngine::Renderer2D::DrawQuad({ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureBarrel);
	StarEngine::Renderer2D::DrawQuad({ -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, m_TextureStairs);
	StarEngine::Renderer2D::DrawQuad({ 1.0f, 0.0f, 0.0f }, { 1.0f, 2.0f }, m_TextureTree);
	StarEngine::Renderer2D::EndScene();

	m_ParticleSystem.OnUpdate(ts);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());
}

void Sandbox2D::OnImGuiRender()
{
	SE_PROFILE_FUNCTION();

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;

#ifdef SE_DIST
	SE_CORE_INFO("FPS : {0}", io.Framerate);
#endif


#ifdef SE_DEBUG

	//Camera Info
	ImGui::Begin("Camera Info");

	ImGui::Text("Camera Position");
	ImGui::Text("X: %.2f", m_CameraController.GetCamera().GetPosition().x);
	ImGui::Text("Y: %.2f", m_CameraController.GetCamera().GetPosition().y);
	ImGui::Text("Z: %.2f", m_CameraController.GetCamera().GetPosition().z);

	ImGui::Text("Camera Rotation: %.2f", m_CameraController.GetCamera().GetRotation());

	if (ImGui::Button("Reset")) {
		m_CameraController.GetCamera().SetPosition(glm::vec3(0.0f));
		m_CameraController.GetCamera().SetRotation(0.0f);
	}

	//Color Picker
	ImGui::Text("Color Picker");

	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	if (ImGui::Checkbox("VSync", &m_VSync)) {
		// Toggle VSync when the checkbox is clicked
		StarEngine::Application::Get().GetWindow().SetVSync(m_VSync);
	}

	ImGui::Text("Settings");

	auto stats = StarEngine::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::End();

#endif

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Birth Color", glm::value_ptr(m_Particle.ColorBegin));
	ImGui::ColorEdit4("Death Color", glm::value_ptr(m_Particle.ColorEnd));
	ImGui::DragFloat("Life Time", &m_Particle.LifeTime, 0.1f, 0.0f, 1000.0f);
	ImGui::End();
}

void Sandbox2D::OnEvent(StarEngine::Event& e)
{
	m_CameraController.OnEvent(e);
}
