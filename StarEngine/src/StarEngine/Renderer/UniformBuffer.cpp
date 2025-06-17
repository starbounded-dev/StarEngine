#include "sepch.h"
#include "UniformBuffer.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine {

	RefPtr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return RefPtr<VulkanUniformBuffer>::Create(size, binding);
		}

		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
