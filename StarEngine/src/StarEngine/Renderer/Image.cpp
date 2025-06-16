#include "sepch.h"
#include "StarEngine/Renderer/Image.h"

#include "StarEngine/Core/Application.h"
#include "StarEngine/Renderer/Renderer.h"
#include "StarEngine/Renderer/RendererAPI.h"

#include <nvrhi/nvrhi.h>

namespace StarEngine {
	static std::map<nvrhi::ITexture*, WeakRef<Image2D>> s_ImageReferences;

	Image2D::Image2D(const ImageSpecification& specification)
		: m_Specification(specification)
	{
		SE_CORE_VERIFY(m_Specification.Width > 0 && m_Specification.Height > 0);
	}

	Image2D::~Image2D()
	{
		Release();
	}

	void Image2D::Invalidate()
	{
		RT_Invalidate();
	}

	void Image2D::Release()
	{
		s_ImageReferences.erase(m_Info.ImageHandle.Get());
	}

	int Image2D::GetClosestMipLevel(uint32_t width, uint32_t height) const {

		if (width > m_Specification.Width / 2 || height > m_Specification.Height / 2) {
			return 0;
		}

		int a = glm::log2(glm::min(m_Specification.Width, m_Specification.Height));
		int b = glm::log2(glm::min(width, height));
		return a - b;
	}

	std::pair<uint32_t, uint32_t> Image2D::GetMipLevelSize(int mipLevel) const
	{
		uint32_t width = m_Specification.Width;
		uint32_t height = m_Specification.Height;
		return { width >> mipLevel, height >> mipLevel };
	}

	void Image2D::RT_Invalidate()
	{
		SE_CORE_VERIFY(m_Specification.Width > 0 && m_Specification.Height > 0);
		nvrhi::DeviceHandle device = Application::GetGraphicsDevice();

		Release();

		nvrhi::TextureDesc textureDesc;
		textureDesc.width = m_Specification.Width;
		textureDesc.height = m_Specification.Height;
		textureDesc.mipLevels = m_Specification.Mips;
		textureDesc.format = Utils::NVRHIFormat(m_Specification.Format);
		// Tiling?

		textureDesc.isRenderTarget = m_Specification.Usage == ImageUsage::Attachement;
		textureDesc.isUAV = m_Specification.Usage == ImageUsage::Storage;

		if (m_Specification.Usage == ImageUsage::Storage)
		{
			textureDesc.isUAV = true;
		}

		m_Info.ImageHandle = device->createTexture(textureDesc);

		s_ImageReferences[m_Info.ImageHandle.Get()] = this;

		if (m_Specification.CreateSampler)
		{
			nvrhi::SamplerDesc samplerDesc;
			samplerDesc.minFilter = samplerDesc.magFilter = samplerDesc.mipFilter = !Utils::IsIntegerBased(m_Specification.Format) ? nvrhi::Filter::Linear : nvrhi::Filter::Nearest;
			samplerDesc.addressU = nvrhi::SamplerAddressMode::ClampToEdge;
			samplerDesc.addressV = samplerDesc.addressW = samplerDesc.addressU;

			m_Info.Sampler = device->createSampler(samplerDesc);
		}
	}

	void Image2D::CreatePerLayerImageViews()
	{
		RefPtr<Image2D> instance = this;
		Renderer::Submit([instance]() mutable
			{
				instance->RT_CreatePerLayerImageViews();
			});
	}

	void Image2D::RT_CreatePerLayerImageViews()
	{
		SE_CORE_ASSERT(m_Specification > 1);

		m_PerLayerImageViews.resize(m_Specification.Layers);
		for (uint32_t layer = 0; layer < m_Specification.Layers; layer++)
		{
			nvrhi::TextureSubresourceSet& tss = m_PerLayerImageViews[layer];
			tss.baseMipLevel = 0;
			tss.numMipLevels = m_Specification.Mips;
			tss.baseArraySlice = layer;
			tss.numArraySlices = 1;
		}
	}

	void Image2D::RT_CreatePerSpecificLayerImageViews(const std::vector<uint32_t>& layerIndices)
	{
		SE_CORE_ASSERT(m_Specification.Layers > 1);

		if (m_PerLayerImageViews.empty())
			m_PerLayerImageViews.resize(m_Specification.Layers);

		for (uint32_t layer : layerIndices)
		{
			nvrhi::TextureSubresourceSet& tss = m_PerLayerImageViews[layer];
			tss.baseMipLevel = 0;
			tss.numMipLevels = m_Specification.Mips;
			tss.baseArraySlice = layer;
			tss.numArraySlices = 1;
		}
	}

	const std::map<nvrhi::ITexture*, WeakRef<Image2D>>& Image2D::GetImageRefs()
	{
		return s_ImageReferences;
	}

	void Image2D::SetData(Buffer buffer)
	{
		SE_CORE_VERIFY(m_Specification.Transfer, "Image must be created with ImageSpecification");

		if (buffer)
		{
			nvrhi::IDevice* device = Application::Get().GetWindow().GetDeviceManager()->GetDevice();

			nvrhi::CommandListHandle commandList = device->createCommandList();
			commandList->open();

			commandList->writeTexture(m_Info.ImageHandle, 0, 0, buffer.Data, Utils::GetImageMemoryRowPitch(m_Specification.Format), Utils::IsDepthFormat(m_Specification.Format));

			commandList->close();
			device->executeCommandList(commandList);
		}
	}

	void Image2D::CopyToHostBuffer(Buffer& buffer) const
	{
		SE_CORE_VERIFY(false);
	}
		 
	void ImageView::Invalidate()
	{
		RT_Invalidate();
	}

	void ImageView::RT_Invalidate()
	{
		nvrhi::TextureSubresourceSet& tss = m_TextureSubresourceSet;
		tss.baseMipLevel = m_Specification.Mip;
		tss.numMipLevels = 1;
		tss.baseArraySlice = 0;
		tss.numArraySlices = m_Specification.Image->GetSpecification().Layers;
	}

	ImageView::ImageView(const ImageViewSpecification& specification)
		: m_Specification(specification)
	{
		Invalidate();
	}
}
