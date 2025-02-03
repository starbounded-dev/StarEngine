#include <StarStudio.h>
#include <StarStudio/Core/EntryPoint.h>

#include "Platform/OpenGL/OpenGLShader.h"

#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Sandbox2D.h"

#include "StarStudio/Renderer/OrthographicCameraController.h"
#include "glm/gtc/type_ptr.hpp"

class ExampleLayer : public StarStudio::Layer
{
	public:
		ExampleLayer()
			: Layer("Example"), m_CameraController(1280.0f / 720.0f, true)
		{
			m_VertexArray = StarStudio::VertexArray::Create();

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

			m_SquareVA = StarStudio::VertexArray::Create();

			float squareVertices[5 * 4] = {
				-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 
				 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
				 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
				-0.5f,  0.5f, 0.0f ,0.0f, 1.0f
			};

			StarStudio::Ref<StarStudio::VertexBuffer> squareVB;
			squareVB.reset(StarStudio::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
			squareVB->SetLayout({
				{ StarStudio::ShaderDataType::Float3, "a_Position" },
				{ StarStudio::ShaderDataType::Float2, "a_TexCoord" }
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

			m_Shader = StarStudio::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

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
			m_FlatColorShader = StarStudio::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

			auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

			m_Texture = StarStudio::Texture2D::Create("assets/textures/Checkerboard.png");
			m_starLogTexture = StarStudio::Texture2D::Create("assets/textures/starLogo.png");

			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(textureShader)->Bind();
			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);

		}

		void OnUpdate(StarStudio::Timestep ts) override
		{
			// Update
			m_CameraController.OnUpdate(ts);

			// Render
			StarStudio::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			StarStudio::RenderCommand::Clear();

			StarStudio::Renderer::BeginScene(m_CameraController.GetCamera());

			glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->Bind();
			std::dynamic_pointer_cast<StarStudio::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

			/*for (int y = 0; y < 20; y++)
			{
				for (int x = 0; x < 20; x++)
				{
					glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
					glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;

					StarStudio::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
				}
			}*/

			auto textureShader = m_ShaderLibrary.Get("Texture");

			m_Texture->Bind();
			StarStudio::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			m_starLogTexture->Bind();
			StarStudio::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

			//triangle
			//StarStudio::Renderer::Submit(m_Shader, m_VertexArray);

			StarStudio::Renderer::EndScene();
		}

		virtual void OnImGuiRender() override
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

		void OnEvent(StarStudio::Event& e) override
		{
			m_CameraController.OnEvent(e);
		}

private:
	StarStudio::ShaderLibrary m_ShaderLibrary;
	StarStudio::Ref<StarStudio::Shader> m_Shader;
	StarStudio::Ref<StarStudio::VertexArray> m_VertexArray;

	StarStudio::Ref<StarStudio::Shader> m_FlatColorShader;
	StarStudio::Ref<StarStudio::VertexArray> m_SquareVA;

	StarStudio::Ref<StarStudio::Texture2D> m_Texture, m_starLogTexture;

	StarStudio::OrthographicCameraController m_CameraController;

	glm::vec3 m_SquareColor = { 0.2f, 0.2f, 0.2f};
};

class Sandbox : public StarStudio::Application
{
public:
	Sandbox()
	{
		// PushLayer(new ExampleLayer());
		PushLayer(new Sandbox2D());
	}

	~Sandbox()
	{

	}

};

StarStudio::Application* StarStudio::CreateApplication()
{
	return new Sandbox();
}