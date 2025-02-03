#include "Sandbox2D.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	:Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f)
{
	
}

void Sandbox2D::OnAttach()
{
	m_SquareVA = StarStudio::VertexArray::Create();
	float squareVertices[5 * 4] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};
	StarStudio::Ref<StarStudio::VertexBuffer> squareVB;
	squareVB.reset(StarStudio::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
	squareVB->SetLayout({
		{ StarStudio::ShaderDataType::Float3, "a_Position" }
		});
	m_SquareVA->AddVertexBuffer(squareVB);
	uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
	StarStudio::Ref<StarStudio::IndexBuffer> squareIB;
	squareIB.reset(StarStudio::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
	m_SquareVA->SetIndexBuffer(squareIB);
	m_FlatColorShader = StarStudio::Shader::Create("assets/shaders/FlatColor.glsl");
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
	StarStudio::Renderer::BeginScene(m_CameraController.GetCamera());
	std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->Bind();
	std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat4("u_Color", m_SquareColor);
	StarStudio::Renderer::Submit(m_FlatColorShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
	StarStudio::Renderer::EndScene();
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

	ImGui::End();
	
	//Color Picker
	ImGui::Begin("Color Picker");

	ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));

	ImGui::End();
	
}

void Sandbox2D::OnEvent(StarStudio::Event& e)
{
	m_CameraController.OnEvent(e);
}
