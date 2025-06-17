#include "sepch.h"
#include "StarEngine/Renderer/VertexArray.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine {

	RefPtr<VertexArray> VertexArray::Create() {

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported") return nullptr;
			case RendererAPI::API::Vulkan:	return RefPtr<VulkanVertexArray>::Create();
		}

		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
