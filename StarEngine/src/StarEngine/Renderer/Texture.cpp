#include "sepch.h"
#include "StarEngine/Renderer/Texture.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine
{
	RefPtr<Texture2D> Texture2D::Create(const TextureSpecification& specification, Buffer data)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return RefPtr<VulkanTexture2D>::Create(specification, data);
		}

		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
