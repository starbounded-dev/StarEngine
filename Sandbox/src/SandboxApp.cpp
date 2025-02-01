#include <StarStudio.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public StarStudio::Layer
{
	public:
		ExampleLayer()
			: Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
		{
			m_VertexArray.reset(StarStudio::VertexArray::Create());

			float vertices[3 * 7] = {
				-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
				 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
				 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
			};

			StarStudio::Ref<StarStudio::VertexBuffer> vertexBuffer;
			vertexBuffer.reset(StarStudio::VertexBuffer::Create(vertices, sizeof(vertices)));
			StarStudio::BufferLayout layout = {
				{ StarStudio::ShaderDataType::Float3, "a_Position" },
				{ StarStudio::ShaderDataType::Float4, "a_Color" }
			};
			vertexBuffer->SetLayout(layout);
			m_VertexArray->AddVertexBuffer(vertexBuffer);

			uint32_t indices[3] = { 0, 1, 2 };
			StarStudio::Ref<StarStudio::IndexBuffer> indexBuffer;
			indexBuffer.reset(StarStudio::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
			m_VertexArray->SetIndexBuffer(indexBuffer);

			m_SquareVA.reset(StarStudio::VertexArray::Create());

			float squareVertices[3 * 4] = {
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

			std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

			std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

			m_Shader.reset(StarStudio::Shader::Create(vertexSrc, fragmentSrc));

			std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

			std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;

			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

			m_FlatColorShader.reset(StarStudio::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

		}

		void OnUpdate(StarStudio::Timestep ts) override
		{
			if (StarStudio::Input::IsKeyPressed(SS_KEY_LEFT))
				m_CameraPosition.x -= m_CameraMoveSpeed * ts;
			else if (StarStudio::Input::IsKeyPressed(SS_KEY_RIGHT))
				m_CameraPosition.x += m_CameraMoveSpeed * ts;

			if (StarStudio::Input::IsKeyPressed(SS_KEY_UP))
				m_CameraPosition.y += m_CameraMoveSpeed * ts;
			else if (StarStudio::Input::IsKeyPressed(SS_KEY_DOWN))
				m_CameraPosition.y -= m_CameraMoveSpeed * ts;

			if (StarStudio::Input::IsKeyPressed(SS_KEY_A))
				m_CameraRotation += m_CameraRotationSpeed * ts;
			if (StarStudio::Input::IsKeyPressed(SS_KEY_D))
				m_CameraRotation -= m_CameraRotationSpeed * ts;

			StarStudio::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			StarStudio::RenderCommand::Clear();

			m_Camera.SetPosition(m_CameraPosition);
			m_Camera.SetRotation(m_CameraRotation);

			StarStudio::Renderer::BeginScene(m_Camera);

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->Bind();
			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

			for (int y = 0; y < 20; y++)
			{
				for (int x = 0; x < 20; x++)
				{
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

					StarStudio::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
				}
			}

			StarStudio::Renderer::Submit(m_Shader, m_VertexArray);

			StarStudio::Renderer::EndScene();
		}

		virtual void OnImGuiRender() override
		{

			//Camera Info
			ImGui::Begin("Camera Info");

			ImGui::Text("Camera Position");
			ImGui::Text("X: %.2f", m_CameraPosition.x);
			ImGui::Text("Y: %.2f", m_CameraPosition.y);
			ImGui::Text("Z: %.2f", m_CameraPosition.z);

			ImGui::Text("Camera Rotation: %.2f", m_CameraRotation);

			if (ImGui::Button("Reset")) {
				m_CameraPosition = glm::vec3(0.0f);
				m_CameraRotation = 0.0f;
			}

			ImGui::End();

			//Color Picker
			ImGui::Begin("Color Picker");

			ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
			if (ImGui::Button("Reset"))
			{
				m_SquareColor = { 0.2f, 0.2f, 0.2f};
			}

			ImGui::End();
		}

		void OnEvent(StarStudio::Event& event) override
		{

		}

private:
	StarStudio::Ref<StarStudio::Shader> m_Shader;
	StarStudio::Ref<StarStudio::VertexArray> m_VertexArray;

	StarStudio::Ref<StarStudio::Shader> m_FlatColorShader;
	StarStudio::Ref<StarStudio::VertexArray> m_SquareVA;

	StarStudio::OrthographicCamera m_Camera;
	glm::vec3 m_CameraPosition;
	float m_CameraMoveSpeed = 5.0f;

	float m_CameraRotation = 0.0f;
	float m_CameraRotationSpeed = 180.0f;

	glm::vec3 m_SquareColor = { 0.2f, 0.2f, 0.2f};
};

class Sandbox : public StarStudio::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

StarStudio::Application* StarStudio::CreateApplication()
{
	return new Sandbox();
}