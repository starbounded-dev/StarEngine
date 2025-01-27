#include "sspch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace StarStudio
{
	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI();
}