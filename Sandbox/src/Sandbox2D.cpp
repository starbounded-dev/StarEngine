#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_internal.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
	
}

void Sandbox2D::OnAttach()
{
	SS_PROFILE_FUNCTION();

	m_CheckerboardTexture = StarStudio::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	SS_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(StarStudio::Timestep ts)
{
	SS_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	{
		SS_PROFILE_SCOPE("Renderer Prep");
		StarStudio::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		StarStudio::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		SS_PROFILE_SCOPE("Renderer Draw");
		StarStudio::Renderer2D::BeginScene(m_CameraController.GetCamera());
		StarStudio::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		StarStudio::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		StarStudio::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		StarStudio::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		StarStudio::Renderer2D::DrawRotatedQuad({ -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }, rotation, m_CheckerboardTexture, 20.0f);
		StarStudio::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	SS_PROFILE_FUNCTION();

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;

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
		StarStudio::Application::Get().GetWindow().SetVSync(m_VSync);
	}

	ImGui::End();
	
}

void Sandbox2D::OnEvent(StarStudio::Event& e)
{
	m_CameraController.OnEvent(e);
}
