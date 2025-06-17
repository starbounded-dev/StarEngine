#include "sepch.h"

#include "StarEngine/Core/Ref.h"

#include "StarEngine/Renderer/Renderer.h"
#include "StarEngine/Renderer/Renderer2D.h"

#include "StarEngine/Audio/AudioEngine.h"

namespace StarEngine {

	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::Init()
	{
		SE_PROFILE_FUNCTION();

		AudioEngine::Init();

		RenderCommand::Init();
		Renderer2D::Init();
	}

	void Renderer::Shutdown()
	{
		Renderer2D::Shutdown();

		AudioEngine::Shutdown();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::BeginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const RefPtr<Shader>& shader, const RefPtr<VertexArray>& vertexArray, const glm::mat4& transform)
	{
		shader->Bind();

		shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetMat4("u_Transform", transform);

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
