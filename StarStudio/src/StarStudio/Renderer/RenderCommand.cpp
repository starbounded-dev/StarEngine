#include "sspch.h"
#include "StarStudio/Renderer/RenderCommand.h"

namespace StarStudio
{
	Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}