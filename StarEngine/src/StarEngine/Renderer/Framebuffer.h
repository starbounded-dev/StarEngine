#pragma once

#include <nvrhi/nvrhi.h>
#include <vector>
#include <unordered_map>

namespace StarEngine {

	class IView;

	class Framebuffer : public RefCounted
	{
	private:
		nvrhi::DeviceHandle m_Device;
		std::unordered_map<nvrhi::TextureSubresourceSet, nvrhi::FramebufferHandle> m_FramebufferCache;

	public:
		Framebuffer(nvrhi::IDevice* device) : m_Device(device) {}
		virtual ~Framebuffer() = default;

		std::vector<nvrhi::TextureHandle> RenderTargets;
		nvrhi::TextureHandle DepthTarget;
		nvrhi::TextureHandle ShadingRateSurface;

		virtual nvrhi::IFramebuffer* GetFramebuffer(const nvrhi::TextureSubresourceSet& subresources);
		nvrhi::IFramebuffer* GetFramebuffer(const IView& view);
	};
}

