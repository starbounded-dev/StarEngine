#include "sspch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace StarStudio {

	VertexArray* VertexArray::Create() {

		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:	SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported") return nullptr;
			case RendererAPI::API::OpenGL:	return new OpenGLVertexArray();
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}