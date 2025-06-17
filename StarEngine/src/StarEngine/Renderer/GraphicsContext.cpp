#include "sepch.h"
#include "StarEngine/Renderer/GraphicsContext.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine
{
	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    SE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::Vulkan:  return std::make_unique<VulkanContext>(static_cast<GLFWwindow*>(window));
		case RendererAPI::API::OpenGL:  return std::make_unique<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		SE_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
