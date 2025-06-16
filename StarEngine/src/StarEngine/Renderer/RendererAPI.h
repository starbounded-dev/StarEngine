#pragma once

#include "RendererCapabilites.h"
#include "RenderCommandBuffer.h"
#include "StorageBufferSet.h"
#include "UniformBufferSet.h"
#include "PipelineCompute.h"
#include "ComputePass.h"
#include "RenderPass.h"

namespace StarEngine {

	class SceneRenderer;

	enum class RendererAPIType
	{
		None,
		Vulkan
	};

	enum class PrimitiveType
	{
		None = 0,
		Triangles, 
		Lines
	};

	class RendererAPI
	{
	public:
		virtual RendererCapabilites& GetCapabilities() = 0;

		static RendererAPI Current() { return s_CurrentRendererAPI; }
		static void SetAPI(RendererAPIType api);

	private:
		inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;
	};
}
