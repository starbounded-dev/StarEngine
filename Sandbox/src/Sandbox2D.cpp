#include "Sandbox2D.h"

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui_internal.h"

#include <chrono>

template<typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func) : m_Name(name), m_Func(func), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if (!m_Stopped)
			Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({ m_Name, duration });
	};

private:
	const char* m_Name;
	Fn m_Func;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped;
};

#define PROFILE_SCOPE(name) Timer timer##__LINE__(name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })

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
	PROFILE_SCOPE("Sandbox2D::OnUpdate");
	// Update
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(ts);
	}
	// Render
	{
		PROFILE_SCOPE("Renderer Prep");
		StarStudio::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		StarStudio::RenderCommand::Clear();
	}

	{
		PROFILE_SCOPE("Renderer2D::BeginScene");
		StarStudio::Renderer2D::BeginScene(m_CameraController.GetCamera());
		StarStudio::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		StarStudio::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, { 0.2f, 0.3f, 0.8f, 1.0f });
		StarStudio::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture);
		StarStudio::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
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

	for (auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}

	ImGui::End();
	
}

void Sandbox2D::OnEvent(StarStudio::Event& e)
{
	m_CameraController.OnEvent(e);
}
