#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f, true)
{
	
}

void Sandbox2D::OnAttach()
{
	m_CheckerboardTexture = StarStudio::Texture2D::Create("assets/textures/Checkerboard.png");
}

void Sandbox2D::OnDetach()
{
	
}

void Sandbox2D::OnUpdate(StarStudio::Timestep ts)
{
	// Update
	m_CameraController.OnUpdate(ts);
	// Render
	StarStudio::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
	StarStudio::RenderCommand::Clear();

	StarStudio::Renderer2D::BeginScene(m_CameraController.GetCamera());
	StarStudio::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
	StarStudio::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
	StarStudio::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);
	StarStudio::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
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

	ImGui::Text("Frame Counter");

	ImGui::Text("FPS: %.u", ImGui::GetIO().Framerate);

	ImGui::End();
	
}

void Sandbox2D::OnEvent(StarStudio::Event& e)
{
	m_CameraController.OnEvent(e);
}
