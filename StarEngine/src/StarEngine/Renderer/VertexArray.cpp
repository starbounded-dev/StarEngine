#include "sepch.h"
#include "StarEngine/Renderer/VertexArray.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine {

	Ref<VertexArray> VertexArray::Create() {

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported") return nullptr;
			case RendererAPI::API::Vulkan:	return Ref<VulkanVertexArray>::Create();
		}

		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
