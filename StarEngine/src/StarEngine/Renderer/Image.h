#pragma once

#include "StarEngine/Core/Base.h"
#include "StarEngine/Core/Ref.h"

#include "StarEngine/Core/Buffer.h"

#include "StarEngine/Renderer/RendererResource.h"

#include <nvrhi/nvrhi.h>

#include <string>

#include <glm/glm.hpp>

namespace StarEngine {

	enum class ImageFormat
	{
		None = 0,
		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,

		RG8,
		RG16F,
		RG32F,

		RGB,
		RGB8,
		RGBA,

		SRGB,
		SRGBA,

		RGBA16F,
		RGBA32F,

		B10R11G11F,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		// Special formats
		Depth = DEPTH24STENCIL8,
	};

	enum class ImageUsage
	{
		None = 0,
		Texture,
		Attachement,
		Storage,
		HostRead
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp, 
		Repeat
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest,
		Cubic
	};

	enum class TextureType
	{
		None = 0,
		Texture2D,
		TextureCube
	};

	struct ImageSpecification {
		std::string DebugName;

		ImageFormat Format = ImageFormat::RGBA;
		ImageUsage Usage = ImageUsage::Texture;

		bool Transfer = false; // Will it be used for transfers ops

		uint32_t Width = 1;
		uint32_t Height = 1;
		uint32_t Layers = 1;
		uint32_t Mips = 1;

		bool CreateSampler = true;
	};

	struct ImageSubresourceRange
	{
		uint32_t BaseMip = 0;
		uint32_t MipCount = UINT_MAX;
		uint32_t BaseLayer = 0;
		uint32_t LayerCount = UINT_MAX;
	};

	union ImageClearValue
	{
		glm::vec4 FloatValues;
		glm::ivec4 IntValues;
		glm::uvec4 UIntValues;
	};

	struct ImageInfo
	{
		nvrhi::TextureHandle ImageHandle = nullptr;
		nvrhi::TextureSubresourceSet ImageView;
		nvrhi::SamplerHandle Sampler = nullptr;
	};

	class Image : public RendererResource
	{
	public:
		virtual void Resize(glm::uvec2& size) = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;
		virtual void Invalidate() = 0;
		virtual void Release() = 0;

		virtual nvrhi::TextureHandle GetHandle() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual glm::uvec2 GetSize() const = 0;
		virtual bool HasMips() const = 0;

		virtual float GetAspectRatio() const = 0;

		virtual ImageSpecification& GetSpecification() = 0;
		virtual const ImageSpecification& GetSpecification() const = 0;
		
		virtual Buffer& GetBuffer() const = 0;
		virtual const Buffer GetBuffer() = 0;

		virtual uint64_t GetGPUMemoryUsage() const = 0;
	
		virtual void CreatePerLayerImageViews() = 0;

		virtual uint64_t GetHash() const = 0;

		virtual void SetData(Buffer buffer) = 0;
		virtual void CopyToHostBuffer(Buffer& buffer) const = 0;

	public:
		virtual ~Image() = default;
	};


	class Image2D : public Image
	{
	public:
		static Ref<Image2D> Create(const ImageSpecification& specification) {
			return Ref<Image2D>::Create(specification);
		}

		bool IsValid() const { return m_Info.ImageHandle != nullptr; }

		virtual void Resize(const glm::uvec2& size) override
		{
			Resize(size.x, size.y);
		}

		virtual void Resize(const uint32_t width, const uint32_t height) override
		{
			m_Specification.Width = width;
			m_Specification.Height = height;
			Invalidate();
		}

		virtual void Invalidate() override;
		virtual void Release() override;

		virtual nvrhi::TextureHandle GetHandle() const override { return m_Info.ImageHandle; }
		virtual uint32_t GetWidth() const override { return m_Specification.Width; }
		virtual uint32_t GetHeight() const override { return m_Specification.Height; }
		virtual glm::uvec2 GetSize() const override { return { m_Specification.Width, m_Specification.Height }; }
		virtual bool HasMips() const override { return m_Specification.Mips > 1; }

		virtual float GetAspectRatio() const override { return (float)m_Specification.Width / (float)m_Specification.Height; }

		int GetClosestMipLevel(uint32_t width, uint32_t height) const;
		std::pair<uint32_t, uint32_t> GetMipLevelSize(int mipLevel) const;

		virtual ImageSpecification& GetSpecification() override { return m_Specification; }
		virtual const ImageSpecification& GetSpecification() const override { return m_Specification; }

		void RT_Invalidate();

		virtual void CreatePerLayerImageViews() override;
		void RT_CreatePerLayerImageViews();
		void RT_CreatePerSpecificLayerImageViews(const std::vector<uint32_t>& layerIndices);

		virtual nvrhi::TextureSubresourceSet GetLayerImageView(uint32_t layer)
		{
			SE_CORE_ASSERT(layer < m_PerLayerImageViews.size(), "Layer index out of bounds!");
			return m_PerLayerImageViews[layer];
		}

		nvrhi::TextureSubresourceSet GetMipImageView(uint32_t mip);
		nvrhi::TextureSubresourceSet RT_GetMipImageView(uint32_t mip);

		ImageInfo& GetImageInfo() { return m_Info; }
		const ImageInfo& GetImageInfo() const { return m_Info; }

		virtual ResourceDescriptorInfo GetDescriptorInfo() const override { return (ResourceDescriptorInfo)&m_Info; }

		virtual Buffer GetBuffer() const override { return m_ImageData;  }
		virtual Buffer& GetBuffer() override { return m_ImageData; }

		virtual uint64_t GetGPUMemoryUsage() const override { return m_GPUAllocationSize; }

		virtual uint64_t GetHash() const override { return (uint64_t)m_Info.ImageHandle->getNativeObject(nvrhi::ObjectType::Texture); }

		// DEBUG
		static const std::map<nvrhi::ITexture*, WeakRef<Image2D>>& GetImageRefs();

		virtual void SetData(Buffer buffer) override;
		virtual void CopyToHostBuffer(Buffer& buffer) const override;
	public:
		Image2D(const ImageSpecification& specification);

		virtual ~Image2D() override;
	private:
		ImageSpecification m_Specification;
		ImageInfo m_Info;

		Buffer m_ImageData;
		uint64_t m_GPUAllocationSize = 0;

		std::vector<nvrhi::TextureSubresourceSet> m_PerLayerImageViews;
		std::map<uint32_t, nvrhi::TextureSubresourceSet> m_PerMipImageViews;
	};

	namespace Utils {

		inline uint32_t GetImageFormatBPP(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RED8UN: return 1;
			case ImageFormat::RED8UI: return 2;
			case ImageFormat::RED16UI: return 2;
			case ImageFormat::RED32UI: return 4;
			case ImageFormat::RED32F: return 4;
			case ImageFormat::RGB:
			case ImageFormat::SRGB: return 3;
			case ImageFormat::RGBA: return 4;
			case ImageFormat::SRGBA: return 4;
			case ImageFormat::RGBA16F: return 2 * 4;
			case ImageFormat::RGBA32F: return 4 * 4;
			case ImageFormat::B10R11G11F: return 4;
			}
			SE_CORE_ASSERT(false, "Unknown ImageFormat!");
			return 0;
		}

		inline bool IsIntegerBased(const ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RED16UI:
			case ImageFormat::RED32UI:
			case ImageFormat::RED8UI:
			case ImageFormat::DEPTH32FSTENCIL8UINT:
				return true;
			case ImageFormat::DEPTH32F:
			case ImageFormat::RED8UN:
			case ImageFormat::RGBA32F:
			case ImageFormat::B10R11G11F:
			case ImageFormat::RED32F:
			case ImageFormat::RGB8:
			case ImageFormat::RGB:
			case ImageFormat::SRGB:
			case ImageFormat::RGBA:
			case ImageFormat::RGBA16F:
			case ImageFormat::SRGBA:
			case ImageFormat::DEPTH24STENCIL8:
				return false;
			}

			SE_CORE_ASSERT(false, "Unknown ImageFormat!");
			return false;
		}

		inline nvrhi::Format NVRHIFormat(ImageFormat format)
		{
			switch (format)
			{
			case ImageFormat::RED8UN: return nvrhi::Format::R8_UNORM;
			case ImageFormat::RED8UI: return nvrhi::Format::R8_UINT;
			case ImageFormat::RED16UI: return nvrhi::Format::R16_UINT;
			case ImageFormat::RED32UI: return nvrhi::Format::R32_UINT;
			case ImageFormat::RED32F: return nvrhi::Format::R32_FLOAT;
			case ImageFormat::RG8: return nvrhi::Format::RG8_UNORM;
			case ImageFormat::RG16F: return nvrhi::Format::RG16_FLOAT;
			case ImageFormat::RG32F: return nvrhi::Format::RG32_FLOAT;
			case ImageFormat::RGBA: return nvrhi::Format::RGBA8_UNORM;
			case ImageFormat::SRGBA: return nvrhi::Format::SRGBA8_UNORM;
			case ImageFormat::RGBA16F: return nvrhi::Format::RGBA16_FLOAT;
			case ImageFormat::RGBA32F: return nvrhi::Format::RGBA32_FLOAT;
			case ImageFormat::B10R11G11F: return nvrhi::Format::R11G11B10_FLOAT;
			case ImageFormat::DEPTH32FSTENCIL8UINT: return nvrhi::Format::D32S8;
			case ImageFormat::DEPTH32F: return nvrhi::Format::D32;
			case ImageFormat::DEPTH24STENCIL8: return nvrhi::Format::D24S8;
			}

			SE_CORE_ASSERT(false, "Unknown ImageFormat!");
			return nvrhi::Format::UNKNOWN;
		}

		inline uint32_t CalculateMipCount(uint32_t wdith, uint32_t height)
		{
			return (uint32_t)glm::floor(glm::log2(glm::min(wdith, height))) + 1;
		}

		inline uint32_t GetImageMemorySize(ImageFormat format, uint32_t width, uint32_t height)
		{
			return width * height * GetImageFormatBPP(format);
		}

		inline uint32_t GetImageMemoryRowPitch(ImageFormat format, uint32_t width)
		{
			return width * GetImageFormatBPP(format);
		}	

		inline bool IsDepthFormat(ImageFormat format)
		{
			if (format == ImageFormat::DEPTH32F ||
				format == ImageFormat::DEPTH24STENCIL8 ||
				format == ImageFormat::DEPTH32FSTENCIL8UINT)
			{
				return true;
			}

			return false;
		}
	}

	struct ImageViewSpecification
	{
		Ref<Image> Image;
		uint32_t Mip = 0;

		std::string DebugName;
	};

	class ImageView : public RendererResource
	{
	public:
		static Ref<ImageView> Create(const ImageViewSpecification& specification);

		void Invalidate();
		void RT_Invalidate();

		const nvrhi::TextureSubresourceSet& GetImageView() const { return m_TextureSubresourceSet; }
	public:
		ImageView(const ImageViewSpecification& specification);

		virtual ~ImageView() = default;
	private:
		ImageViewSpecification m_Specification;

		nvrhi::TextureSubresourceSet m_TextureSubresourceSet;
	};
}
