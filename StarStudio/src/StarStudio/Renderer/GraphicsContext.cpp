#include "sspch.h"
#include "StarStudio/Renderer/GraphicsContext.h"

#include "StarStudio/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"

namespace StarStudio
{
	Scope<GraphicsContext> GraphicsContext::Create(void* window)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}