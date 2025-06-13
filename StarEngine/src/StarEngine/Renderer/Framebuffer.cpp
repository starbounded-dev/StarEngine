#include "sepch.h"
#include "StarEngine/Renderer/Framebuffer.h"

#include "StarEngine/Renderer/Renderer.h"

namespace StarEngine
{
	nvrhi::IFramebuffer* Framebuffer::GetFramebuffer(const nvrhi::TextureSubresourceSet& subresources)
	{
		nvrhi::FramebufferHandle& item = m_FramebufferCache[subresources];

		if (!item)
		{
			nvrhi::FramebufferDesc desc;
			for (auto renderTarget : RenderTargets)
				desc.addColorAttachment(renderTarget, subresources);

			if (DepthTarget)
				desc.setDepthAttachment(DepthTarget, subresources);

			if (ShadingRateSurface)
				desc.setShadingRateAttachment(ShadingRateSurface, subresources);

			item = m_Device->createFramebuffer(desc);
		}

		return item;
	}

	nvrhi::IFramebuffer* Framebuffer::GetFramebuffer(const IView& view)
	{
		return GetFramebuffer(view.GetSubresources());
	}
}
