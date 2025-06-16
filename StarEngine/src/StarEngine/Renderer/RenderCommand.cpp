#include "sepch.h"
#include "StarEngine/Renderer/RenderCommand.h"

namespace StarEngine
{
	std::unique_ptr<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}
