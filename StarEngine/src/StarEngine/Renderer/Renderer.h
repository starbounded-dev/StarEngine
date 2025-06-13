#pragma once

#include "StarEngine/Renderer/RenderCommand.h"

#include "StarEngine/Renderer/OrthographicCamera.h"
#include "StarEngine/Renderer/Shader.h"

namespace StarEngine {

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t heighta);

		static void BeginScene(OrthographicCamera& camera);
		static void EndScene();

		static void Submit(const RefPtr<Shader>& shader, const RefPtr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};
		static std::unique_ptr<SceneData> s_SceneData;
	};
}
