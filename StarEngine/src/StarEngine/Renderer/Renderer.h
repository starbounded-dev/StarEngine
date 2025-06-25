#pragma once

#include "RendererContext.h"
#include "RenderCommandQueue.h"
#include "RenderPass.h"
#include "ComputePass.h"
#include "RenderCommandBuffer.h"
#include "PipelineCompute.h"
#include "UniformBufferSet.h"
#include "StorageBufferSet.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Core/RenderThread.h"

#include "RendererCapabilities.h"
#include "RendererConfig.h"

#include "GPUStats.h"

#include "StarEngine/Scene/Scene.h"

namespace StarEngine {

	class ShaderLibrary;

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
